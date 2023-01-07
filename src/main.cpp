#include <all.h>
#define DEBUG

std::mt19937 rnd(time(0));

int main() {
    GLFWwindow* window = GLFW_Prework();
    Shader Texture_Shader("texture");
    Texture my = ResourceManager::GetTexture("my.png");
    Texture floor = ResourceManager::GetTexture("floor.bmp");
    Texture ban = ResourceManager::GetTexture("ban.bmp");
    Texture enemy = ResourceManager::GetTexture("enemy.bmp");
    Texture knife = ResourceManager::GetTexture("knife.png");
    Texture bullet_tex = ResourceManager::GetTexture("bullet.png");
    Creature My(&Texture_Shader, &my, "player", 0, -yMax + my.height / 2 + floor.height + 1);
    Bullet Knife(&Texture_Shader,&knife,"player",0,0);
    int lstt = -1e9, atkt = -1e9;
    int enemyt = 0;

    int levelx=0,levely=0;
    auto press = [&](int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };
    while (!glfwWindowShouldClose(window)) {
        if(press(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);
        if(levelx==0) {
            // Game Start
            if(press(GLFW_KEY_C)) {
                levelx = 1; levely = 0; lstt = -1e9, atkt = -1e9; enemyt=clock();
                std::tie(My.xpos,My.ypos)=getMap(levelx,levely,&Texture_Shader,&ban,&enemy);
                My.health = 20;
            }
            // render

        } else if(1<=levelx&&levelx<=5) {
            // input
            // -----
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
                    Knife.ypos = My.ypos; atkt = clock(); Atk = 1;
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
            
            if(Knifepd) {
                for(auto &b:MapManager::Bullets) if(b.type!=1) {
                    if(!chk_PP(Con(b),Con(Knife))) {
                        if(b.type==0) b.type = 1, b.velx = -b.velx;
                        if(b.type==2) b.use = 1;
                    }
                }
            }

            calcDamage(My);
            for(auto &c:MapManager::Creatures) {
                calcDamage(c);
                // if(Atk) std::cerr<<"!"<<std::endl;
                if(Atk&&(!chk_PP(Con(c),Con(Knife)))) c.health-=7;
            }
            if(My.health<=0) {
                levelx=7,levely=0; continue ;
                // Game Over
            }
            std::vector<Creature>_C;
            for(auto &c:MapManager::Creatures) if(c.health>0) _C.emplace_back(c);
            MapManager::Creatures.swap(_C);
            std::vector<Bullet>_B;
            for(auto &b:MapManager::Bullets) if(!b.use) _B.emplace_back(b);
            MapManager::Bullets.swap(_B);
            if(_C.empty()) {
                if(levely) {
                    levely = 0;
                    levelx++;
                } else {
                    levely = 1;
                }
                if(levelx<=5) {
                    lstt = -1e9, atkt = -1e9; enemyt=clock();
                    std::tie(My.xpos,My.ypos)=getMap(levelx,levely,&Texture_Shader,&ban,&enemy);
                }
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
        } else if(levelx==7) {
            // Game Over
            if(press(GLFW_KEY_C)) {
                levelx = 1; levely = 0; lstt = -1e9, atkt = -1e9; enemyt=clock();
                std::tie(My.xpos,My.ypos)=getMap(levelx,levely,&Texture_Shader,&ban,&enemy);
                My.health = 20;
            }
        } else if(levelx==6) {
            // pass game
            if(press(GLFW_KEY_C)) {
                levelx = 1; levely = 0; lstt = -1e9, atkt = -1e9; enemyt=clock();
                std::tie(My.xpos,My.ypos)=getMap(levelx,levely,&Texture_Shader,&ban,&enemy);
                My.health = 20;
            }
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
