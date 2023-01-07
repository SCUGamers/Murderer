#include <all.h>
#include <Windows.h>
#define DEBUG

std::mt19937 rnd(time(0));

int main() {
    ShowWindow(GetConsoleWindow(),SW_HIDE);
    stbi_set_flip_vertically_on_load(true);
    GLFWwindow* window = GLFW_Prework();
    Shader Texture_Shader("texture");
    Texture my = ResourceManager::GetTexture("my.png");
    Texture ban = ResourceManager::GetTexture("block.png");
    Texture enemy = ResourceManager::GetTexture("enemy.png");
    Texture knife = ResourceManager::GetTexture("knife.png");
    Texture bullet_tex = ResourceManager::GetTexture("bullet.png");
    Texture heart_tex =  ResourceManager::GetTexture("heart.png");
    Creature My(&Texture_Shader, &my, "player", 0, 0);
    Bullet Knife(&Texture_Shader,&knife,"player",0,0);
    Item heart(&Texture_Shader,&heart_tex,0,290);
    int lstt = -1e9, atkt = -1e9;
    int enemyt = 0;
    int Ctime = -1e9;
    int levelx=0,levely=0;
    int leveltim = -1e9;
    auto press = [&](int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };
    while (!glfwWindowShouldClose(window)) {
        if(press(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);
        if(levelx==0) {
            // Game Start
            if(press(GLFW_KEY_C)&&(clock()-Ctime)>200) {
                Ctime=clock();
                levelx = 1; levely = 0; lstt = -1e9, atkt = -1e9; enemyt=clock();
                std::tie(My.xpos,My.ypos)=getMap(levelx,&Texture_Shader,&ban,&enemy);
                leveltim=clock();
                My.health = 20; continue ;
            }
            // render
            Texture t=ResourceManager::GetTexture("gamestart.png");
            Item back(&Texture_Shader,&t,0,0);
            back.draw();
        } else if(1<=levelx&&levelx<=5) {
            if(levely==1) {
                // input
                // -----
                if(press(GLFW_KEY_R)&&(clock()-Ctime)>200) {
                    Ctime=clock();
                    lstt = -1e9, atkt = -1e9; enemyt=clock();
                    std::tie(My.xpos,My.ypos)=getMap(levelx,&Texture_Shader,&ban,&enemy);
                    leveltim=clock();
                    My.health = 20; continue ;
                }
                if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                    My.velx=-1; My.face=1;
                }
                if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                    My.velx=1; My.face=0;
                }
                bool Float = checkfloat(My), move = 0;

                if(clock() - lstt > 10) move = 1, lstt = clock();
                if(move) {
                    if(Float) My.vely+=My.accy; else My.vely = 0;
                    for(auto &c:MapManager::Creatures) if(checkfloat(c)) c.vely+=c.accy; else c.vely=0;
                }
                if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
                    if(!Float) My.vely = 7;
                }
                bool Atk=0, Knifepd=((clock()-atkt)<200);
                if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
                    if(clock() - atkt > 500) {
                        atkt = clock(); Atk = 1;
                        Knife.ypos = My.ypos;
                        if(My.face) Knife.xpos = My.xpos - My.X - Knife.X;
                        else Knife.xpos =  My.xpos + My.X + Knife.X;
                    }
                }

                if(clock()-enemyt>1000) { // enemy atk
                    enemyt=clock();
                    for(auto &c:MapManager::Creatures) {
                        float disx;
                        if(c.face) disx=-200; else disx=200;
                        Con_Hul Box=Mer(Con(c),Con(c,disx,0));
                        if(!chk_PP(Box,Con(My))) {
                            MapManager::Bullets.emplace_back(&Texture_Shader,&bullet_tex,"enemy",c.xpos,c.ypos);
                            auto ptr=MapManager::Bullets.rbegin();
                            if(c.face) ptr->velx=-2; else ptr->velx=2;
                        }
                    }
                }
                if(move) {
                    // player move
                    // std::cerr<<"vel"<<Float<<' '<<My.velx<<' '<<My.vely<<std::endl;
                    float dy=fixMovementY(My);
                    float dx=fixMovementX(My);
                    // std::cerr<<dx<<' '<<dy<<std::endl;
                    My.xpos+=dx,My.ypos+=dy;
                    My.velx=0;
                    // enemy move
                    for(auto &c:MapManager::Creatures) {
                        bool turn=(rnd()%20==0);
                        if(turn) c.face^=1;
                        if(c.face) { // left
                            c.velx = -0.5;
                        } else {
                            c.velx = 0.5;
                        }
                        checkfloat(c);
                        float disy=fixMovementY(c);
                        float disx=fixMovementX(c);
                        c.xpos+=disx,c.ypos+=disy;
                        if(syn(disx)==0) c.face^=1;
                        c.velx=0;
                    }
                    // bullet move
                    for(auto &b:MapManager::Bullets) {
                        float dis=fixMovement(b);
                        if(syn(dis)==0) b.use=1;
                        b.xpos+=dis;
                    }
                }
                
                if(Knifepd&&levelx!=5) {
                    Knife.ypos = My.ypos;
                    if(My.face) Knife.xpos = My.xpos - My.X - Knife.X;
                    else Knife.xpos =  My.xpos + My.X + Knife.X;
                    for(auto &b:MapManager::Bullets) if(b.type!=1) {
                        if(!chk_PP(Con(b),Con(Knife))) {
                            if(b.type==0) b.type = 1, b.velx = -b.velx;
                            if(b.type==2) b.use = 1;
                        }
                    }
                }

                if(levelx==4) calcDamage(My,7); else calcDamage(My);
                for(auto &c:MapManager::Creatures) {
                    calcDamage(c);
                    // if(Atk) std::cerr<<"!"<<std::endl;
                    if(Atk&&(!chk_PP(Con(c),Con(Knife)))) c.health-=7;
                }
                if((levelx==2||levelx==4)&&(clock()-leveltim)>5000) {
                    My.health--;
                }
                if(levelx==3&&(clock()-leveltim)>3000) {
                    My.health--;
                }

                if(My.health<=0) {
                    levelx=7; continue ;
                    // Game Over
                }
                std::vector<Creature>_C;
                for(auto &c:MapManager::Creatures) if(c.health>0) _C.emplace_back(c); else My.health=std::min(My.health+3,20);
                MapManager::Creatures.swap(_C);
                std::vector<Bullet>_B;
                for(auto &b:MapManager::Bullets) if(!b.use) _B.emplace_back(b);
                MapManager::Bullets.swap(_B);
                if(_C.empty()) {
                    levely++;
                    continue ;
                }

                // render
                // ------
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                My.draw();
                if(Knifepd) {
                    Knife.draw();
                }
                for(auto &I:MapManager::Items) I.draw();
                for(auto &E:MapManager::Creatures) E.draw();
                for(auto &B:MapManager::Bullets) B.draw();
                for(int i=0;i<My.health;i++) heart.xpos=-390+20*i,heart.draw();
            } else if(levely==0) {
                // rule page
                if(press(GLFW_KEY_C)&&(clock()-Ctime)>200) {
                    Ctime=clock();
                    levely++; continue ;
                }
                // render
                Texture t=ResourceManager::GetTexture("rule"+std::to_string(levelx)+".png");
                Item back(&Texture_Shader,&t,0,0);
                back.draw();
            } else if(levely==2) {
                // next page
                if(press(GLFW_KEY_C)&&(clock()-Ctime)>200) {
                    Ctime=clock();
                    levelx++; levely = 0;
                    if(levelx <= 5) {
                        lstt = -1e9, atkt = -1e9; enemyt=clock();
                        std::tie(My.xpos,My.ypos)=getMap(levelx,&Texture_Shader,&ban,&enemy);
                        My.health = 20; leveltim=clock();
                    } continue ;
                }
                // render
                Texture t=ResourceManager::GetTexture("levelpass.png");
                Item back(&Texture_Shader,&t,0,0);
                back.draw();
            } else assert(0);
        } else if(levelx==7) {
            // Game Over
            if(press(GLFW_KEY_C)&&(clock()-Ctime)>200) {
                Ctime=clock();
                levelx = 0; levely = 0;
            }
            // render
            Texture t=ResourceManager::GetTexture("gameover.png");
            Item back(&Texture_Shader,&t,0,0);
            back.draw();
        } else if(levelx==6) {
            // pass game
            if(press(GLFW_KEY_C)&&(clock()-Ctime)>200) {
                Ctime=clock();
                levelx = 0; levely = 0;
            }
            // render
            Texture t=ResourceManager::GetTexture("gamepass.png");
            Item back(&Texture_Shader,&t,0,0);
            back.draw();
        } else assert(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
