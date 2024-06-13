#include <raylib.h>

struct AnimData 
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    //update running time 
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        //update anim frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame)
        {   
            data.frame = 0;
        }
    }
    return data;
}
int main()
{
    const int windowDimensions[2]{512, 380};

    InitAudioDevice();
    Music music = LoadMusicStream("music/dapper_dasher_theme.wav");
    PlayMusicStream(music);
    Sound jumpSound = LoadSound("sounds/dapper-dasher-jump-grunt.wav");
    Sound deathSound = LoadSound("sounds/dapper-dasher-death-grunt.wav");
    SetSoundVolume(jumpSound, 0.5);
    SetSoundVolume(deathSound, 0.5);


    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher");

    SetTargetFPS(60);

    //gravity in pixels per second/s
    const int gravity = 1000;

    //scarfy
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0]/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/6.0;
    scarfyData.runningTime = 0;


    //nebula setup
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");


    const int sizeOfNebulae{10};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].pos.x = windowDimensions[0] + i * 300;    
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0/16.0;
    }

    //finish line setup
    float finishLine = nebulae[sizeOfNebulae - 1].pos.x;
 

    //nebula velocty pixels/s
    int nebVel = -200;

    int velocity = 0;

    //background setup
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};

    //midground setup
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};

    //foreground setup
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    bool isInAir = false;
    //jump velocity pixels/s
    const int  jumpVel = -600;

    //collision variable
    bool collision = false;

    while(!WindowShouldClose())
    {
        UpdateMusicStream(music);
        
        //time since last frame
        const float dT = GetFrameTime();
        scarfyData.runningTime += dT; 

        for(int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i].runningTime += dT;
        }
   
        

        BeginDrawing();

        ClearBackground(WHITE);

        bgX -= 20 * dT;
        if (bgX <= -background.width*2)
        {
            bgX = 0.0;
        }
        mgX -= 40 * dT;
        if (mgX <= -midground.width*2)
        {
            mgX = 0.0;
        }
        fgX -= 80 * dT;
        if (fgX <= -foreground.width*2)
        {
            fgX = 0.0;
        }
        //draw background 
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);
        //draw midground
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);
        //draw foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        //ground check
        if(isOnGround(scarfyData, windowDimensions[1]))
        {
            velocity = 0;
            isInAir = false;
        }    
        else
        {
            velocity += gravity * dT;
            isInAir = true;
        }
        
        //jump check
        if(IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            if(!collision)
            {
                PlaySound(jumpSound);
            }
            velocity += jumpVel;
        }

        for(int i = 0; i < sizeOfNebulae; i++)
        {
            //update nebulae position
            nebulae[i].pos.x += nebVel * dT;
        }

        //collision check
        for (AnimData nebula : nebulae)
        {
            const float pad = 50;
            Rectangle nebRec 
            {
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };
            Rectangle scarfyRec
            {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };

            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }
        //update finish line
        finishLine += nebVel * dT;

        //update scarfy position
        scarfyData.pos.y += velocity * dT;

        //update scarfy anim if not in air
        if (!isInAir)
        {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }
        //update nebulae anims
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }


        if (!collision)
        {
            if(scarfyData.pos.x >= finishLine)
            {
                DrawText("You Win!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
            }
            else
            {
                for(int i = 0; i < sizeOfNebulae; i++)
                {
                    DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
                }

                //draw scarfy
                DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
            }

        }
        else
        {
            DrawText("Game Over!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
        }


        EndDrawing();

    }
    StopMusicStream(music);
    UnloadSound(jumpSound);
    UnloadSound(deathSound);
    UnloadMusicStream(music);
    CloseAudioDevice();
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}