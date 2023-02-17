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
    Client* clientWhoMakeTheOrder{ nullptr }; //Qui à fait la commande, et du coup à qui il faudra l'apporter

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
            if (find(indexes.begin(), indexes.end(), ingIndex) == indexes.end()) //Si on ne trouve pas l'index(donc que l'iterator est égal à la fin du vecteur)
            {
                indexes.push_back(ingIndex);
            }
        }
        for (int var : indexes)
        {
            newOrder.ingredients.push_back(AllIngrediens.at(var));

        }
        newOrder.clientWhoMakeTheOrder = this;
        orderMade = newOrder;
        cout << "Client make order:\n ";
        newOrder.DisplayOrder();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        orderPromise->set_value(newOrder);

        
        return newOrder;

    }

    promise<Order>* orderPromise; //Promesse faite au seveur de lui donner le plat qu'il veut 


private:

    Order orderMade;

};

class Waiter
{
public:
    //Savoir quand une commande à été faite par un client
    void WaitingOrderFromClient()
    {
        /*
        std::promise<Order> promiseOrderFromClient;  //Créer la promesse
        future<Order> futurOrderFromClient = promiseOrderFromClient.get_future();   //On s'engage à avoir une value
        clientTest.orderPromise = &promiseOrderFromClient;
        thread waiterWaiForOrderThread(&Waiter::ReceiveOrder, this, ref(futurOrderFromClient));   //Envoie le futur dans un thread
        */
    }


    void ReceiveOrder(future<Order>& orderReceive)
    {
        Order order = orderReceive.get();
        cout << "Waiter receive odrer from client" << " \n ";
        order.DisplayOrder();

        std::this_thread::sleep_for(std::chrono::seconds(2));
        //On peut mettre un delay pour le temps d'ammener la commande
        giveOrderToChiefPromise->set_value(order);


    }
    void GiveOrder(future<Order>& orderReceive)
    {
        giveOrderToClient->set_value(orderReceive.get());
    }
        //L'ammener au chef
    //Savoir quand une commande à été cuinier par le chef
        //Apporter la commande à qui l'a faite
    promise<Order>* giveOrderToChiefPromise{nullptr};
    promise<Order>* giveOrderToClient{nullptr};

};

class Cooker
{
public:
    Cooker() {};
    ~Cooker() {};
    //Reçoit un ordre d'ingredient de la part du chef: listen ordre du chef
    //Prépare son ingredient
    //L'envoie au chef quand c'est finit
    void ReceiveIngrendientsDemand(future<vector<Ingredient>>& ingredientsrReceive)
    {
        vector<Ingredient> ingredients = ingredientsrReceive.get();
        cout << "Cooker receive ingredients from chief" << " \n ";
        cout << "Receive: " << ingredients.at(0).Name << " + " << ingredients.at(1).Name << " + "<< ingredients.at(2).Name <<" \n " << endl;
    }

    void GiveIngredients(vector<Ingredient>& ingredientsrReceive)
    {
        //Il y aura une promesse faite au chef de lui donner des ingredients
        //Du coup on va set la valeur de cette promesse avec la liste des ingrédients finit 
        PromiseToChiefToGiveIngredients->set_value(ingredientsrReceive);
    }

    promise<vector<Ingredient>>* PromiseToChiefToGiveIngredients{ nullptr };

};

class Chief
{
public:
    //Reçoit une commande du serveur
    void ReceiveOrder(future<Order>& orderReceive)
    {
        Order order = orderReceive.get();
        cout << "Chief receive order from waiter" << " \n ";
        order.DisplayOrder();
        orderSaved = order;

        //
        if (!cookers.empty())
        {
            vector<vector<Ingredient>> ingredientsNeeded = DivideList<Ingredient>(order.ingredients, cookers.size());
            //Assigner à chaque cooker une liste d'ingrédients à cuisiner

            for (size_t i = 0; i < cookers.size(); i++)
            {
                //Set la valeur de la promesse de l'ingrédient
                std::this_thread::sleep_for(std::chrono::seconds(3));

                PromiseToCookersToGiveIngredients->set_value(ingredientsNeeded.at(i));
            }

        }
    }

    void ReceiveIngredients(future<vector<Ingredient>>& ingredientsReceive)
    {
        vector<Ingredient> ingred = ingredientsReceive.get();
        for (size_t i = 0; i < ingred.size(); i++)
        {
            ingredints.push_back(ingred.at(i));
        }
        cout << "Chief receive ingredient from cooker" << " \n ";
        
    }

    void MakingOrder()
    {
        Order newOrder;
        for (auto it = ingredints.begin(); it < ingredints.end(); it++)
        {
            newOrder.ingredients.push_back(*it);
        }
        newOrder.clientWhoMakeTheOrder = orderSaved.clientWhoMakeTheOrder;
        //Va prendre tout les ingredients des cuisiniers, et en faire un Order 
        //Va avoir promesse fait au serveur de lui donner un plat, va donc set value de la promesse
        PromiseToWaiterToGiveOrder->set_value(newOrder);


        orderSaved = Order();
        ingredints.clear();
    }


    vector<Ingredient> ingredints;
    Order orderSaved;

    vector<Cooker*> cookers{  };
    promise<vector<Ingredient>>* PromiseToCookersToGiveIngredients{ nullptr };//Promess aux cuisiniers de leur dire quel ingrédient
    promise<Order>* PromiseToWaiterToGiveOrder{ nullptr };//Promess aux cuisiniers de leur dire quel ingrédient
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
    std::promise<Order> promiseOrderFromClient;  //Créer la promesse
    future<Order> futurOrderFromClient = promiseOrderFromClient.get_future();   //On s'engage à avoir une value
    clientTest.orderPromise = &promiseOrderFromClient;
    thread waiterWaiForOrderThread(&Waiter::ReceiveOrder, &waiterTest, ref(futurOrderFromClient));   //Envoie le futur dans un thread

    //----------[Chef qui attend de reçevoir la commande
    //Promess sur le waiter 
    std::promise<Order> promiseOrderFromChief;  //Créer la promesse
    future<Order> futurOrderFromChief = promiseOrderFromChief.get_future();   //On s'engage à avoir une value
    waiterTest.giveOrderToChiefPromise = &promiseOrderFromChief;       //Pour faire sa promesse
    thread chiefWaiForOrderThread(&Chief::ReceiveOrder, &chiefTest, ref(futurOrderFromChief));   //Envoie le futur dans un thread

    //----------[Cuisinier qui attend de reçevoir les ordre du chef
    std::promise<vector<Ingredient>> promiseOrderFromCooker;  //Créer la promesse
    future<vector<Ingredient>> futurOrderFromCooker = promiseOrderFromCooker.get_future();   //On s'engage à avoir une value
    chiefTest.PromiseToCookersToGiveIngredients = &promiseOrderFromCooker;
    thread cookerWaiForOrderThread(&Cooker::ReceiveIngrendientsDemand, &cookerTest, ref(futurOrderFromCooker));   //Envoie le futur dans un thread


    //----------[Chef qui attend les ingredients des cookers
    /*
    std::promise<vector<Ingredient>> promiseOrderFromCooker;  //Créer la promesse
    future<vector<Ingredient>> futurOrderFromCooker = promiseOrderFromCooker.get_future();   //On s'engage à avoir une value
    chiefTest.PromiseToCookersToGiveIngredients = &promiseOrderFromCooker;
    thread cookerWaiForOrderThread(&Cooker::ReceiveIngrendientsDemand, &cookerTest, ref(futurOrderFromCooker));   //Envoie le futur dans un thread
    */


    //----------[Serveur qui attend de reçevoir le plat du cuisinier

      //----------[Client qui attend de reçevoir son plat
    /*
    std::promise<Order> promiseWaiterToClient;  //Créer la promesse
    future<Order> futurOrderFromWaiter = promiseWaiterToClient.get_future();   //On s'engage à avoir une value
    waiterTest.giveOrderToClient = &promiseOrderFromClient;
    waiterWaiForOrderThread = thread(&Waiter::ReceiveOrder, &waiterTest, ref(futurOrderFromClient));   //Envoie le futur dans un thread
    */


    clientTest.MakeOrder();


    waiterWaiForOrderThread.join();
    chiefWaiForOrderThread.join();
    cookerWaiForOrderThread.join();


}

