#ifndef ALL_H
#define ALL_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include <bits/stdc++.h>
#include "geometry.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const int xMax = SCR_WIDTH / 2, yMax = SCR_HEIGHT / 2;

const char Game_Name[]="Game";

// Shader Texture_Shader("texture");

// classes

class Texture {
public:
    unsigned int ID;
    int width, height, nrChannels;
    Texture() : ID(-1u) {}
    Texture(const char *pic) {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 加载并生成纹理
        static char picpath[512]; sprintf(picpath,"./resources/pic/%s",pic);
        unsigned char *data = stbi_load(picpath, &width, &height, &nrChannels, 0);
        std::cerr << width << ' ' << height << ' ' << nrChannels << std::endl;
        if (data) {
            if(nrChannels == 4)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else if(nrChannels == 3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else std::cout << "Failed to load texture" << std::endl;
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
};

namespace ResourceManager {
    // Resource storage
    std::map<std::string, Texture> Textures;
    // Retrieves a stored texture
    Texture GetTexture(std::string name) {
        if(Textures.find(name) == Textures.end())
            Textures[name] = Texture(name.c_str());
        return Textures[name];
    }
}

float ver[32]={
    0, 0, 0, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0, 0, 0, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0, 0, 0, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0, 0, 0, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f
};
class Item {
public:
    unsigned int VAO,VBO,EBO;
    float xpos, ypos, X, Y;
    Shader* shader;
    Texture* tex;
    Item(Shader* sp, Texture *tp, float x, float y) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // glGenBuffers(1, &EBO);
        xpos = x;
        ypos = y;
        tex = tp;
        X = tp -> width / 2;
        Y = tp -> height / 2;
        shader = sp;
    }
    void draw() {
        // shader prepare
        shader -> use();
        shader -> setInt("ourTexture", 0);

        ver[0 ] = (xpos - X) / (float)xMax, ver[1 ] = (ypos - Y) / (float)yMax;
        ver[8 ] = (xpos - X) / (float)xMax, ver[9 ] = (ypos + Y) / (float)yMax;
        ver[16] = (xpos + X) / (float)xMax, ver[17] = (ypos - Y) / (float)yMax;
        ver[24] = (xpos + X) / (float)xMax, ver[25] = (ypos + Y) / (float)yMax;

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ver), ver, GL_STATIC_DRAW);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex -> ID);
        glBindVertexArray(VAO);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 1, 3);

    }
};

class Creature : public Item {
public:
    float velx;
    float vely, accy;
    bool face;
    int health;
    int type; // 0 : enemy ; 1 : player
    Creature(Shader* sp, Texture *tp, std::string t, float x, float y):
        Item(sp,tp,x,y),velx(0),vely(0),accy(0),health(20),face(0),
        type(t=="player") {}
};

class Bullet : public Item {
public:
    int type; // 0 : enemy ; 1 : player ; 2 : env
    bool use;
    float velx;
    int atk = 5;
    Bullet(Shader* sp, Texture *tp, std::string t, float x, float y):
        Item(sp,tp,x,y),velx(0),atk(5),
        type(t=="env" ? 2 : (t=="player")),use(0) {}
};

// common function
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

GLFWwindow* GLFW_Prework() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, Game_Name, NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    return window;
}

// core function

namespace MapManager {
    std::vector<Item> Items;
    std::vector<Creature> Creatures;
    std::vector<Bullet> Bullets;
    bool Map[30][40];
}

std::pair<int,int> getMap(int x,Shader *sp,Texture *bp,Texture *ep) {
    FILE* fptr = fopen(("./resources/map/"+std::to_string(x)).c_str(),"r");
    MapManager::Items.clear();
    MapManager::Creatures.clear();
    MapManager::Bullets.clear();
    for(int i=0;i<30;i++) for(int j=0;j<40;j++) MapManager::Map[i][j]=0;
    char buf[64]; int posx,posy;
    for(int i=1;i<30;i++) {
        fgets(buf,sizeof(buf),fptr);
        assert(strlen(buf)==41);
        for(int j=0;j<40;j++) {
            float Y=10+(14-i)*20,X=10+(j-20)*20; char c=buf[j];
            if(c=='*') {
                MapManager::Items.emplace_back(sp,bp,X,Y);
                MapManager::Map[i][j]=1;
                // std::cerr<<"#"<<i<<' '<<j<<std::endl;
            } else if(c=='E') {
                MapManager::Creatures.emplace_back(sp,ep,"enemy",X,Y+1);
            } else if(c=='P') {
                posx=X,posy=Y+1;
            }
        }
    }
    return {posx,posy};
}

// logic

Con_Hul Con(Item &c,float dx=0,float dy=0) {
    return std::vector<Point>{
        {c.xpos-c.X+dx,c.ypos-c.Y+dy},
        {c.xpos-c.X+dx,c.ypos+c.Y-1+dy},
        {c.xpos+c.X-1+dx,c.ypos+c.Y-1+dy},
        {c.xpos+c.X-1+dx,c.ypos-c.Y+dy}
    };
}

Con_Hul Mer(Con_Hul A,Con_Hul B) {
    A.insert(A.end(),B.begin(),B.end());
    return Andrew(A);
}

float fixMovementX(Creature &c) {
    if(syn(c.velx)==0) return 0;
    int xl=std::max(0,(int)floor((c.xpos+390.)/20.)),xr=(int)ceil((c.xpos+390.)/20.),
        yl=std::max(0,(int)floor((290.-c.ypos)/20.)),yr=(int)ceil((290.-c.ypos)/20.);
    float nx=c.xpos+c.velx;
    xl=std::min(xl,std::max(0,(int)floor((nx+390)/20.)));
    xr=std::max(xr,(int)ceil((nx+390.)/20.));
    xl=std::max(0,xl-1); xr=std::min(39,xr+1); yl=std::max(0,yl-1); yr=std::min(29,yr+1);
    Con_Hul MoveCrashBox=Mer(Con(c),Con(c,c.velx,0));
    for(int i=yl;i<=yr;i++) for(int j=xl;j<=xr;j++) if(MapManager::Map[i][j]) {
        float Y=10+(14-i)*20,X=10+(j-20)*20;
        if(!chk_PP(MoveCrashBox,std::vector<Point>{{X-10,Y-10},{X-10,Y+9},{X+9,Y+9},{X+9,Y-10}})) {
            c.velx = 0; break ;
        } 
    }
    return c.velx;
}
float fixMovementY(Creature &c) {
    if(syn(c.vely)==0) return 0;
    int xl=std::max(0,(int)floor((c.xpos+390.)/20.)),xr=(int)ceil((c.xpos+390.)/20.),
        yl=std::max(0,(int)floor((290.-c.ypos)/20.)),yr=(int)ceil((290.-c.ypos)/20.);
    float ny=c.ypos+c.vely;
    yl=std::min(yl,std::max(0,(int)floor((290.-ny)/20.)));
    yr=std::max(yr,(int)ceil((290.-ny)/20.));
    xl=std::max(0,xl-1); xr=std::min(39,xr+1); yl=std::max(0,yl-1); yr=std::min(29,yr+1);

    Con_Hul MoveCrashBox=Mer(Con(c),Con(c,0,c.vely));
    for(int i=yl;i<=yr;i++) for(int j=xl;j<=xr;j++) if(MapManager::Map[i][j]) {
        // if(c.type) std::cerr<<"!"<<i<<' '<<j<<std::endl;
        float Y=10+(14-i)*20,X=10+(j-20)*20;
        if(!chk_PP(MoveCrashBox,std::vector<Point>{{X-10,Y-10},{X-10,Y+9},{X+9,Y+9},{X+9,Y-10}})) {
            c.vely = c.accy = 0; break ;
        } 
    }
    return c.vely;
}


float fixMovement(Bullet &c) {
    if(syn(c.velx)==0) return 0;
    int xl=std::max(0,(int)floor((c.xpos+390.)/20.)),xr=(int)ceil((c.xpos+390.)/20.),
        yl=std::max(0,(int)floor((290.-c.ypos)/20.)),yr=(int)ceil((290.-c.ypos)/20.);
    float nx=c.xpos+c.velx;
    xl=std::min(xl,std::max(0,(int)floor((nx+390.)/20.)));
    xr=std::max(xr,(int)ceil((nx+390.)/20.));
    xl=std::max(0,xl-1); xr=std::min(39,xr+1); yl=std::max(0,yl-1); yr=std::min(29,yr+1);

    Con_Hul MoveCrashBox=Mer(Con(c),Con(c,c.velx,0));
    for(int i=yl;i<=yr;i++) for(int j=xl;j<=xr;j++) if(MapManager::Map[i][j]) {
        float Y=10+(14-i)*20,X=10+(j-20)*20;
        if(!chk_PP(MoveCrashBox,std::vector<Point>{{X-10,Y-10},{X-10,Y+9},{X+9,Y+9},{X+9,Y-10}})) {
            c.velx = 0; break ;
        } 
    }
    return c.velx;
}

bool checkfloat(Creature &c) {
    float nvy = c.vely; c.vely = -1;
    float d = fixMovementY(c);
    if(d != 0) c.accy = -0.25; else c.accy = 0;
    c.vely = nvy;
    return c.accy != 0;
}

void calcDamage(Creature &c,int Atk=-1) {
    Con_Hul MoveCrash=Con(c);
    for(auto &B: MapManager::Bullets) if(!B.use&&B.type!=c.type&&!chk_PP(MoveCrash,Con(B))) {
        if(Atk==-1) c.health -= B.atk; else c.health-=Atk;
        std::cerr<<"!"<<c.type<<' '<<c.health<<std::endl;
        B.use = 1;
    }
}
// display function

#endif