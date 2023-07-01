import requests
import sys

def get_bitcoin_price():
    url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd"
    response = requests.get(url)

    if response.status_code == 200:
        data = response.json()
        bitcoin_price = data["bitcoin"]["usd"]
        print(bitcoin_price)
    else:
        print("Error: Unable to retrieve data from CoinGecko API.")
        sys.exit(1)

if __name__ == "__main__":
    get_bitcoin_price()
