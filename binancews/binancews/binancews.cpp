


#include <iostream>
#include <future>

#include <cpprest/ws_client.h>
#include <cpprest/json.h>

#include <BinanceExchange.h>
#include <Logger.hpp>
#include <Redis.hpp>

using namespace std::chrono_literals;

/*
template <typename T>
std::string toString(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}
*/


using namespace binancews;


int main(int argc, char** argv)
{
    try
    {
        // create and connect to Redis
        auto redis = std::make_shared<Redis>();
        redis->init("172.22.253.65", 7379);



        auto onAllSymbolsDataFunc = [redis] (std::map<std::string, std::string> data)
        {
            static string ChannelNameStart = "binance_";
            static string ChannelNameEnd = "_EXCHANGE_INSTRUMENT_PRICE_CHANNEL";

            std::stringstream ss;
            ss << "Publishing " << data.size() << " symbol updates";
            logg(ss.str());


            for (const auto& sym : data)
            {
                web::json::value exchangeValue ;
                exchangeValue[L"exchange"] = web::json::value{ L"binance" };

                web::json::value instrumentValue;
                instrumentValue[L"instrument"] = web::json::value{ std::wstring{sym.first.cbegin(), sym.first.cend()} };

                web::json::value priceValue;
                priceValue[L"price"] = web::json::value{ std::wstring{sym.second.cbegin(), sym.second.cend()} };

                web::json::value val;
                val[0] = exchangeValue;
                val[1] = instrumentValue;
                val[2] = priceValue;

                auto wideString = val.serialize();
                auto asString = std::string{ wideString.cbegin(), wideString.cend() };

                redis->publish(ChannelNameStart + sym.first + ChannelNameEnd, asString);
            }
        };


        auto consoleFuture = std::async(std::launch::async, []()
        {
            bool run = true;
            std::string cmd;
            while (run && std::getline(std::cin, cmd))
            {
                run = (cmd != "stop");
            }
        });


        Binance be;
        if (auto allSymbolsToken = be.monitorAllSymbols(onAllSymbolsDataFunc) ;  allSymbolsToken.isValid())
        {
            consoleFuture.wait();
        }
        else
        {
            logg("Failed to create monitor for All Symbols");
        }        
    }
    catch (const std::exception ex)
    {
        logg(ex.what());
    }

    
    return 0;
}