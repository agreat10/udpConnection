#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Network.hpp>

#include <cmath>
#include <thread>
#include <vector>
#include <algorithm>

#include <UdpCommunication.h>

char buffer1[1024] = "";
char buffer2[1024] = "";

void display(UdpCommunication &c1, UdpCommunication &c2)
{
    ImGui::Begin("Window 1");

    ImGui::Text(std::string("Last message => " + c1.GetMessage()).c_str());
    ImGui::Separator();
    ImGui::InputText("input text c1", buffer1, IM_ARRAYSIZE(buffer1));

    if(ImGui::Button("Send message to c2"))
    {
        c1.SendMessage(std::string(buffer1));
    }

    ImGui::End();



    ImGui::Begin("Window 2");

    ImGui::Text(std::string("Last message => " + c2.GetMessage()).c_str());
    ImGui::Separator();
    ImGui::InputText("input text c2", buffer2, IM_ARRAYSIZE(buffer2));
    if(ImGui::Button("Send nessage to c1"))
    {
        c2.SendMessage(buffer2);
    }

    ImGui::End();
}


int main()
{
    UdpCommunication c1(1891, "127.0.0.1", 1892);
    UdpCommunication c2(1892, "127.0.0.1", 1891);

    sf::RenderWindow window(sf::VideoMode(800, 600), "Arduino UDP");
    window.setFramerateLimit(60);

    if(ImGui::SFML::Init(window))
    {
        sf::Clock deltaClock;
        while (window.isOpen())
        {
            sf::Event event{};
            while (window.pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(window, event);
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }
            ImGui::SFML::Update(window, deltaClock.restart());

            display(c1, c2);

            window.clear();
            ImGui::SFML::Render(window);
            window.display();
        }
        ImGui::SFML::Shutdown();
    }

    return 0;
}
