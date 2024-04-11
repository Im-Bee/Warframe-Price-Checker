import requests
import re
import json

if __name__ == "__main__":
    inputList = []
    open('price.txt', 'w')
    for item in inputList:
        file = open('price.txt', 'a')
        item = re.sub(r'\s', '_', item)
        link = 'https://api.warframe.market/v1/items/'+item+'/orders'
        source = requests.get(link).text
        data = json.loads(source)
        values = data['payload']['orders']
        values.reverse()
        price = []
        amount = 0
        for val in values[0:5:]:
            if val['order_type'] == 'sell':
                price.append(val['platinum'])
                amount = amount + 1
        if amount <= 1:
            amount = 1
        mean = sum(price) / amount
        file.write(f'{mean}\n')
        file.close()


# najniższa cena śrendia
