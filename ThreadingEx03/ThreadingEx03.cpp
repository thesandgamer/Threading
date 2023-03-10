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
    Client* clientWhoMakeTheOrder{ nullptr }; //Qui ? fait la commande, et du coup ? qui il faudra l'apporter

    void DisplayOrder()
    {
        cout << "Receive: ";
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

    void ReveiveFood(future<Order>& orderReceive)
    {
        Order food = orderReceive.get();

        cout << "Client receive food from waiter \n";
        food.DisplayOrder();
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
            if (find(indexes.begin(), indexes.end(), ingIndex) == indexes.end()) //Si on ne trouve pas l'index(donc que l'iterator est ?gal ? la fin du vecteur)
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
    //Savoir quand une commande ? ?t? faite par un client
    void WaitingOrderFromClient()
    {
        /*
        std::promise<Order> promiseOrderFromClient;  //Cr?er la promesse
        future<Order> futurOrderFromClient = promiseOrderFromClient.get_future();   //On s'engage ? avoir une value
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

    /// <summary>
    /// Le serveur re?oit le plat et l'am?ne au client
    /// </summary>
    /// <param name="orderReceive"></param>
    void ReceiveFood(future<Order>& orderReceive)
    {
        Order order = orderReceive.get();
        cout << "Waiter receive food from chief" << " \n ";
        order.DisplayOrder();

        std::this_thread::sleep_for(std::chrono::seconds(2));

        giveOrderToClient->set_value(order);


    }


    promise<Order>* giveOrderToChiefPromise{nullptr};//Promet au chef de lui donner quel plat ? faire
    promise<Order>* giveOrderToClient{nullptr};//Promesse au client de lui donner son plat

};

class Cooker
{
public:
    Cooker() {};
    ~Cooker() {};
    //Re?oit un ordre d'ingredient de la part du chef: listen ordre du chef
    //Pr?pare son ingredient
    //L'envoie au chef quand c'est finit
    void ReceiveIngrendientsDemand(future<vector<Ingredient>>& ingredientsrReceive)
    {
        ingredientsReceive = ingredientsrReceive.get();
        cout << "Cooker receive ingredients from chief" << " \n ";
        cout << "Receive: " << ingredientsReceive.at(0).Name << " + " << ingredientsReceive.at(1).Name << " + "<< ingredientsReceive.at(2).Name <<" \n " << endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));
        GiveIngredients();
    }

    void GiveIngredients()
    {
        //Il y aura une promesse faite au chef de lui donner des ingredients
        //Du coup on va set la valeur de cette promesse avec la liste des ingr?dients finit 
        PromiseToChiefToGiveIngredients->set_value(ingredientsReceive);
        ingredientsReceive.clear();
    }

    vector<Ingredient> ingredientsReceive;
    promise<vector<Ingredient>>* PromiseToChiefToGiveIngredients{ nullptr };//Promet au chef de lui donner l'ingr?dient un fois finit

};

class Chief
{
public:
    //Re?oit une commande du serveur
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
            //Assigner ? chaque cooker une liste d'ingr?dients ? cuisiner

            for (size_t i = 0; i < cookers.size(); i++)
            {
                //Set la valeur de la promesse de l'ingr?dient
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
        cout << "Chief receive ingredients from cooker" << " \n "<<endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));   //Temps pour cuisiner le tout 

        MakingOrder();
        
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

    promise<vector<Ingredient>>* PromiseToCookersToGiveIngredients{ nullptr };//Promess aux cuisiniers de leur dire quel ingr?dient
    promise<Order>* PromiseToWaiterToGiveOrder{ nullptr };//Promess aux cuisiniers de leur dire quel ingr?dient
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
    std::promise<Order> promiseOrderFromClient;  //Cr?er la promesse
    future<Order> futurOrderFromClient = promiseOrderFromClient.get_future();   //On s'engage ? avoir une value
    clientTest.orderPromise = &promiseOrderFromClient;
    thread waiterWaiForOrderThread(&Waiter::ReceiveOrder, &waiterTest, ref(futurOrderFromClient));   //Envoie le futur dans un thread

    //----------[Chef qui attend de re?evoir la commande
    //Promess sur le waiter 
    std::promise<Order> promiseOrderFromChief;  //Cr?er la promesse
    future<Order> futurOrderFromChief = promiseOrderFromChief.get_future();   //On s'engage ? avoir une value
    waiterTest.giveOrderToChiefPromise = &promiseOrderFromChief;       //Pour faire sa promesse
    thread chiefWaiForOrderThread(&Chief::ReceiveOrder, &chiefTest, ref(futurOrderFromChief));   //Envoie le futur dans un thread

    //----------[Cuisinier qui attend de re?evoir les ordre du chef
    std::promise<vector<Ingredient>> promiseOrderFromCooker;  //Cr?er la promesse
    future<vector<Ingredient>> futurOrderFromCooker = promiseOrderFromCooker.get_future();   //On s'engage ? avoir une value
    chiefTest.PromiseToCookersToGiveIngredients = &promiseOrderFromCooker;
    thread cookerWaiForOrderThread(&Cooker::ReceiveIngrendientsDemand, &cookerTest, ref(futurOrderFromCooker));   //Envoie le futur dans un thread


    //----------[Chef qui attend les ingredients des cookers
    
    std::promise<vector<Ingredient>> PromiseChiefIngreduents;  //Cr?er la promesse
    future<vector<Ingredient>> FuturIngredients = PromiseChiefIngreduents.get_future();   //On s'engage ? avoir une value
    cookerTest.PromiseToChiefToGiveIngredients = &PromiseChiefIngreduents;
    thread chiefWaitToReceiveIngredientsThread(&Chief::ReceiveIngredients, &chiefTest, ref(FuturIngredients));   //Envoie le futur dans un thread
    


    //----------[Serveur qui attend de re?evoir le plat du cuisinier
    std::promise<Order> PromiseWaiterToGiveOrderChief;  //Cr?er la promesse
    future<Order> FuturOrder = PromiseWaiterToGiveOrderChief.get_future();   //On s'engage ? avoir une value
    chiefTest.PromiseToWaiterToGiveOrder = &PromiseWaiterToGiveOrderChief;
    thread WaiterWaitOrderFromChief(&Waiter::ReceiveFood, &waiterTest, ref(FuturOrder));   //Appel Receive food quand le futur se fait


      //----------[Client qui attend de re?evoir son plat
    
    std::promise<Order> promiseWaiterToClient;  //Cr?er la promesse
    future<Order> futurOrderFromWaiter = promiseWaiterToClient.get_future();   //On s'engage ? avoir une value
    waiterTest.giveOrderToClient = &promiseWaiterToClient;
    thread clientWaitThread(&Client::ReveiveFood, &clientTest, ref(futurOrderFromWaiter));   //Envoie le futur dans un thread
    


    clientTest.MakeOrder();


    waiterWaiForOrderThread.join();
    chiefWaiForOrderThread.join();
    cookerWaiForOrderThread.join();
    chiefWaitToReceiveIngredientsThread.join();
    WaiterWaitOrderFromChief.join();
    clientWaitThread.join();



}

