#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <cmath>
#include <iostream>

// Development branch

using namespace sf;

const int num=8; //checkpoints
// TODO: use checkpoint to make sure we are on the track.
// Slow speed when not on the track.
int points[num][2] = {300, 610,
    1270,430,
    1380,2380,
    1900,2460,
    1970,1700,
    2550,1680,
    2560,3150,
    500, 3300};

struct Car
{
    float m_x,m_y,m_speed,m_angle; int m_n;
    Car() {m_speed=2; m_angle=0; m_n=0;}
    void move()
    {
        m_x += sin(m_angle) * m_speed;
        m_y -= cos(m_angle) * m_speed;
    }
    void findTarget()
    {
        float tx=points[m_n][0];
        float ty=points[m_n][1];
        float beta = m_angle-atan2(tx-m_x,-ty+m_y);
        if (sin(beta)<0) m_angle+=0.005*m_speed; else m_angle-=0.005*m_speed;
        // Check if passed a checkpoint
        float xHelper=m_x-tx;
        float yHelper=m_y-ty;
        if ((xHelper)*(xHelper)+(yHelper)*(yHelper)<625) m_n=(m_n+1)%num; // TODO: simplify
    }
};

int main()
{
    // ****************************************
    // Initialise
    srand(time(NULL));
    RenderWindow app(VideoMode(1920, 1080), "Car Racing Game!");
    app.setFramerateLimit(60);
    Texture t1,t2;
    t1.loadFromFile("images/background.png");
    t2.loadFromFile("images/car.png");
    t1.setSmooth(true);
    t2.setSmooth(true);
    Sprite sBackground(t1), sCar(t2);
    sBackground.scale(2,2);
    sCar.setOrigin(22, 22);
    float R=22;
    const int N=5;
    Car car[N];
    Color colors[5] = {Color::Red, Color::Green, Color::Magenta, Color::Blue, Color::White};

    // Starting positions
    for(int i=0;i<N;i++)
    {
        car[i].m_x=300+i*50;
        car[i].m_y=1700+i*80;
        car[i].m_speed=7+((float)i/5);
    }
    float speed=0,angle=0;
    float maxSpeed=12.0;
    float acc=0.2, dec=0.3;
    float turnSpeed=0.08;
    int offsetX=0,offsetY=0;


    // ****************************************
    // Loop

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }
        // Step 1: user input
        bool Up=0,Right=0,Down=0,Left=0;
        if (Keyboard::isKeyPressed(Keyboard::Up))    Up=1;
        if (Keyboard::isKeyPressed(Keyboard::Right)) Right=1;
        if (Keyboard::isKeyPressed(Keyboard::Down))  Down=1;
        if (Keyboard::isKeyPressed(Keyboard::Left))  Left=1;
        //
        // Step 2: update
        //car movement
        if (Up && speed<maxSpeed)
        {
            if (speed < 0)  speed += dec;
            else  speed += acc;
        }
        if (Down && speed>-maxSpeed)
        {
            if (speed > 0) speed -= dec;
            else  speed -= acc;
        }
        if (!Up && !Down)
        {
            if (speed - dec > 0) speed -= dec;
            else if (speed + dec < 0) speed += dec;
            else speed = 0;
        }
        if (Right && speed!=0)  angle += turnSpeed * speed/maxSpeed;
        if (Left && speed!=0)   angle -= turnSpeed * speed/maxSpeed;
        car[0].m_speed = speed;
        car[0].m_angle = angle;
        for(int i=0;i<N;i++) car[i].move();
        for(int i=1;i<N;i++) car[i].findTarget();
        //collision
        for(int i=0;i<N;i++)
        {
            for(int j=0;j<N;j++)
            {
                if (i == j)
                {
                    break;
                }
                int dx=0, dy=0;
                while (dx*dx+dy*dy<4*R*R)
                {
                    car[i].m_x+=dx/10.0;
                    car[i].m_x+=dy/10.0;
                    car[j].m_x-=dx/10.0;
                    car[j].m_y-=dy/10.0;
                    dx = car[i].m_x-car[j].m_x;
                    dy = car[i].m_y-car[j].m_y;
                    if (!dx && !dy) break;
                }
            }
        }
        // Step 3: Render
        app.clear(Color::White);
        // TODO: Stay within the limit of the map.
        // TODO: Don't show white at bottom/right.

        // Check if on track test
        RectangleShape reki;
        reki.setSize(sf::Vector2f((points[2][0]-points[1][0])+100,(points[2][1]-points[1][1])+400));
        reki.setOutlineColor(sf::Color::Blue);
        reki.setFillColor(Color::Transparent);
        reki.setOutlineThickness(5);
        reki.setPosition(points[1][0], points[1][1]);
        //
        RectangleShape langa;
        langa.setSize(sf::Vector2f((points[3][0]-points[2][0])+50,(points[3][1]-points[2][1])+110));
        langa.setOutlineColor(sf::Color::Blue);
        langa.setFillColor(Color::Transparent);
        langa.setOutlineThickness(5);
        langa.setPosition(points[2][0], points[2][1]+120);
        //

        if (car[0].m_x>320) offsetX = car[0].m_x-320;
        if (car[0].m_y>240) offsetY = car[0].m_y-240;
        sBackground.setPosition(-offsetX,-offsetY);
        reki.setPosition(-offsetX+points[1][0],-offsetY+points[1][1]-100);
        langa.setPosition(-offsetX+points[2][0],-offsetY+points[2][1]+120);
        app.draw(sBackground);
        for(int i=0;i<N;i++)
        {
            sCar.setPosition(car[i].m_x-offsetX,car[i].m_y-offsetY);
            sCar.setRotation(car[i].m_angle*180/3.141593);
            sCar.setColor(colors[i]);
            app.draw(sCar);
        }

        app.draw(reki);
        app.draw(langa);

        app.display();
    }

    return 0;
}
