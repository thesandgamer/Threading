#include <iostream>
#include <thread>
#include <future>
#include <stdio.h>     
#include <stdlib.h> 
#include <vector>
#include <mutex>
#include <algorithm>
#include <list>
#include <string>
#include <chrono>

#define stringify( name ) #name


using namespace std;
using std::cout;
using std::endl;

struct Order;
class Client;
class Chief;
class Cooker;
class Waiter;

template <class listType >
vector<vector<listType>> DivideList(vector<listType>& list, int dividingNumber = 3)
{
    int old = 0;
    vector<vector<listType>> sublists;

    for (size_t i = 0; i < dividingNumber; i++)
    {
        vector<listType> sublist;
        float endNumber = (i + 1) * (float)((float)list.size() / dividingNumber);
        sublist.assign(list.begin() + old, list.begin() + endNumber);
        sublists.push_back(sublist);
        old = endNumber;
    }
    return sublists;
}


struct Ingredient {
    string Name{ "IngrendientName" };
    float timeToPrepare{1};
};
vector<Ingredient> AllIngrediens{ {"Potato"}, {"Rice"} ,{"Steak"},{"Bannana"},{"Garlic"}, {"Bread"}, {"Onion"},{"Chicken"},{"Carrot"}, {"Bean"} };


struct Order
{
    vector<Ingredient> ingredients;    //De quels ingredients est fait la commande
    Client* clientWhoMakeTheOrder{ nullptr }; //Qui � fait la commande, et du coup � qui il faudra l'apporter

    void DisplayOrder()
    {
        cout << "Reveive: ";
        for (auto it = ingredients.begin(); it != ingredients.end(); ++it)
        {
            cout << (*it).Name;
            if (std::next(it) != ingredients.end())
            {
                cout << " + ";
            }
        }
        cout << "\n" << endl;
    }
};

class Client
{
public:
    Client()
    {
    };
    ~Client()
    {
    };

    void ReceiveOrder(future<Order>& orderReceive)
    {
        cout << "Client receive \n";
    };

    void Eating()
    {

    };

    void FinishEating()
    {

    };

    Order MakeOrder()
    {
        int numberOfIngredientsWanted = 3;
        Order newOrder;
        vector<int> indexes;
        indexes.reserve(numberOfIngredientsWanted);

        while (indexes.size() != numberOfIngredientsWanted)
        {
            int ingIndex = rand() % AllIngrediens.size();
            if (find(indexes.begin(), indexes.end(), ingIndex) == indexes.end()) //Si on ne trouve pas l'index(donc que l'iterator est �gal � la fin du vecteur)
            {
                indexes.push_back(ingIndex);
            }
        }
        for (int var : indexes)
        {
            newOrder.ingredients.push_back(AllIngrediens.at(var));

        }
        newOrder.clientWhoMakeTheOrder = this;

        orderPromise->set_value(newOrder);
        orderMade = newOrder;
        cout << "Client make order:\n ";
        newOrder.DisplayOrder();
        
        return newOrder;

    }

    promise<Order>* orderPromise;


private:

    Order orderMade;

};

class Waiter
{
public:
    //Savoir quand une commande � �t� faite par un client
    void WaitingOrderFromClient()
    {
        /*
        std::promise<Order> promiseOrderFromClient;  //Cr�er la promesse
        future<Order> futurOrderFromClient = promiseOrderFromClient.get_future();   //On s'engage � avoir une value
        clientTest.orderPromise = &promiseOrderFromClient;
        thread waiterWaiForOrderThread(&Waiter::ReceiveOrder, this, ref(futurOrderFromClient));   //Envoie le futur dans un thread
        */
    }


    void ReceiveOrder(future<Order>& orderReceive)
    {
        Order order = orderReceive.get();
        cout << "Waiter receive odrer from client" << " \n ";
        order.DisplayOrder();

        //On peut mettre un delay pour le temps d'ammener la commande
        giveOrderToChiefPromise->set_value(order);


    }
    void GiveOrder(future<Order>& orderReceive)
    {
        giveOrderToClient->set_value(orderReceive.get());
    }
        //L'ammener au chef
    //Savoir quand une commande � �t� cuinier par le chef
        //Apporter la commande � qui l'a faite
    promise<Order>* giveOrderToChiefPromise{nullptr};
    promise<Order>* giveOrderToClient{nullptr};

};

class Cooker
{
public:
    Cooker() {};
    ~Cooker() {};
    //Re�oit un ordre d'ingredient de la part du chef: listen ordre du chef
    //Pr�pare son ingredient
    //L'envoie au chef quand c'est finit
    void ReceiveIngrendientsDemand(future<vector<Ingredient>>& ingredientsrReceive)
    {
        vector<Ingredient> ingredients = ingredientsrReceive.get();
        cout << "Cooker receive ingredients from chief" << " \n ";
        cout << "Receive: " << ingredients.at(0).Name << " + " << ingredients.at(1).Name << " + "<< ingredients.at(2).Name <<" \n " << endl;
    }

};

class Chief
{
public:
    //Re�oit une commande du serveur
    void ReceiveOrder(future<Order>& orderReceive)
    {
        Order order = orderReceive.get();
        cout << "Chief receive order from waiter" << " \n ";
        order.DisplayOrder();

        //
        if (!cookers.empty())
        {
            vector<vector<Ingredient>> ingredientsNeeded = DivideList<Ingredient>(order.ingredients, cookers.size());
            //Assigner � chaque cooker une liste d'ingr�dients � cuisiner

            for (size_t i = 0; i < cookers.size(); i++)
            {
                //Set la valeur de la promesse de l'ingr�dient
                orderPromise->set_value(ingredientsNeeded.at(i));
            }

        }
    }



    vector<Cooker*> cookers{  };
    promise<vector<Ingredient>>* orderPromise{ nullptr };//Promess aux cuisiniers
};


int main()
{
    srand(time(NULL));


    Client clientTest{};
    Waiter waiterTest{};
    Chief chiefTest{};
    Cooker cookerTest{};
    
    chiefTest.cookers.emplace_back(&cookerTest);

    //----------[Serveur qui attend de recevoir la commande du client
    //Promesse sur le client qui MakeOrder
    std::promise<Order> promiseOrderFromClient;  //Cr�er la promesse
    future<Order> futurOrderFromClient = promiseOrderFromClient.get_future();   //On s'engage � avoir une value
    clientTest.orderPromise = &promiseOrderFromClient;
    thread waiterWaiForOrderThread(&Waiter::ReceiveOrder, &waiterTest, ref(futurOrderFromClient));   //Envoie le futur dans un thread

    //----------[Chef qui attend de re�evoir la commande
    //Promess sur le waiter 
    std::promise<Order> promiseOrderFromChief;  //Cr�er la promesse
    future<Order> futurOrderFromChief = promiseOrderFromChief.get_future();   //On s'engage � avoir une value
    waiterTest.giveOrderToChiefPromise = &promiseOrderFromChief;       //Pour faire sa promesse
    thread chiefWaiForOrderThread(&Chief::ReceiveOrder, &chiefTest, ref(futurOrderFromChief));   //Envoie le futur dans un thread

    //----------[Cuisinier qui attend de re�evoir les ordre du chef
    std::promise<vector<Ingredient>> promiseOrderFromCooker;  //Cr�er la promesse
    future<vector<Ingredient>> futurOrderFromCooker = promiseOrderFromCooker.get_future();   //On s'engage � avoir une value
    chiefTest.orderPromise = &promiseOrderFromCooker;
    thread cookerWaiForOrderThread(&Cooker::ReceiveIngrendientsDemand, &cookerTest, ref(futurOrderFromCooker));   //Envoie le futur dans un thread

    //----------[Chef qui attend les ingredients des cookers


    //----------[Client qui attend de re�evoir son plat
    /*
    std::promise<Order> promiseWaiterToClient;  //Cr�er la promesse
    future<Order> futurOrderFromWaiter = promiseWaiterToClient.get_future();   //On s'engage � avoir une value
    waiterTest.giveOrderToClient = &promiseOrderFromClient;
    waiterWaiForOrderThread = thread(&Waiter::ReceiveOrder, &waiterTest, ref(futurOrderFromClient));   //Envoie le futur dans un thread
    */

    //----------[Serveur qui attend de re�evoir le plat du cuisinier

  


    clientTest.MakeOrder();


    waiterWaiForOrderThread.join();
    chiefWaiForOrderThread.join();
    cookerWaiForOrderThread.join();


}
