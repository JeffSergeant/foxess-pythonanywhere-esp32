import requests
import hashlib
import json
from datetime import datetime,timedelta
import os

TOKEN_FILE= 'token.txt'
DEVICE_ID = 'YOUR-DEVICE-ID-HERE'
USERNAME = 'JoeBloggs'
PASSWORD = 'ABCDEFGHI'


def get_headers(token=""):

    headers = {"token":token,"User-Agent": "Mozilla/5.0 (X11; Ubuntu; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36 RuxitSynthetic/1.0 v1712750679142508729 t2919217341348717815",
                       "Accept": "application/json, text/plain, */*",
                       "lang": "en",
                       "sec-ch-ua-platform": "windows",
                       "Sec-Fetch-Site": "same-origin",
                       "Sec-Fetch-Mode": "cors",
                       "Sec-Fetch-Dest": "empty",
                       "Referer": "https://www.foxesscloud.com/bus/device/inverterDetail?id=xyz&flowType=1&status=1&hasPV=true&hasBattery=false",
                       "Accept-Language":"en-US;q=0.9,en;q=0.8,de;q=0.7,nl;q=0.6",
                       "Connection": "keep-alive",
                        "X-Requested-With": "XMLHttpRequest"}
    return headers


def get_token(refresh = False):

    if os.path.exists(TOKEN_FILE) and not refresh:
        try:
            with open(TOKEN_FILE) as f:
                token = f.read()

            if get_raw(token,'"generationPower"')['errno']!=41809:
                return token
            else:
                print("Token Invalid, Continuing")

        except OSError:
            print("Error reading token file continuing")

    url = 'https://www.foxesscloud.com/c/v0/user/login'

    hashedPassword = hashlib.md5(PASSWORD.encode()).hexdigest()
    data = {"user": USERNAME, "password": hashedPassword}

    auth = json.loads(requests.post(url,data=data,headers=get_headers()).content)

    token = auth['result']['token']
    with open(TOKEN_FILE,'w+') as f:
        f.write(token)

    return token


def get_variables(token):
    url = 'https://www.foxesscloud.com/c/v1/device/variables?deviceID=' + DEVICE_ID

    variables= json.loads(requests.get(url,headers=get_headers(token)).content)
    return variables

def get_raw(token, variables, timezone=0,allday=True,days=7):
    url = 'https://www.foxesscloud.com/c/v0/device/history/raw'
    now = datetime.now()

    if days>0:
        now -= timedelta(days-1)

    year= now.strftime("%Y")
    month= now.strftime("%-m")
    day = now.strftime("%-d")

    if allday:
        hour = '0'
        minute = '0'
    else:
        hour = str(int(now.strftime("%-H"))+timezone)
        minute = now.strftime("%-M")

    data = '{"deviceID":"'+DEVICE_ID+'","variables":['+variables+'],"timespan":"hour","beginDate":{"year":' + year +',"month":'+month+',"day":'+day+', "hour":'+hour +',"minute":'+minute+',"second":0}}'

    raw = json.loads(requests.post(url,data=data,headers=get_headers(token)).content)

    return raw


def get_live_data():
    token = get_token()

    variables = get_variables(token)

    var_string = '"' + '","'.join([v['variable'] for v in variables['result']['variables']]) + '"'
    data = get_raw(token,var_string,days=0,allday=False)['result']
    return data


if __name__ == '__main__':

    data = get_live_data()
    print(data[-1]['data'][-1]['time'])
    for r in data:

        print(f"{r['data'][-1]['value']:.3f}\t{r['variable']} ({r['unit']})")


