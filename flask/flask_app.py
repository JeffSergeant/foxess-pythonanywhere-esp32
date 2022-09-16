from flask import Flask,request,render_template,Response
import json
import foxess
app = Flask(__name__)

@app.route('/soc')
def solar_soc():
    push_variables = ['SoC']
    token = foxess.get_token()
    print(token)
    variables = push_variables
    print(variables)
    var_string = '"' + '","'.join(variables) + '"'
    data = foxess.get_raw(token, var_string, days=0, allday=False, timezone=1)['result']

    if not data[0]['data']:
        data = foxess.get_raw(token, var_string, days=0, allday=False, timezone=0)['result']
    try:
        latest_data = {r['variable']:r['data'][-1]['value'] for r in data if r['variable'] in push_variables}
        response = json.dumps(latest_data['SoC'])
    except(TypeError):
        response = '50'

    print(response)
    return Response(response,200)

@app.route('/net')
def solar_net():
    push_variables = ['loadsPower','pvPower','batChargePower']
    token = foxess.get_token()
    print(token)
    variables = push_variables
    print(variables)
    var_string = '"' + '","'.join(variables) + '"'
    data = foxess.get_raw(token, var_string, days=0, allday=False, timezone=1)['result']
    if not data[0]['data']:
        data = foxess.get_raw(token, var_string, days=0, allday=False, timezone=0)['result']

    print(data)
    try:
        latest_data = {r['variable']:r['data'][-1]['value'] for r in data if r['variable'] in push_variables}
        print(latest_data)
        response = float(json.dumps(latest_data['pvPower']))-float(json.dumps(latest_data['loadsPower']))

    except(TypeError):
        response = '1.8'

    print(response)

    response = str(int(max(min((float(response)/1.8),1),-1)*50)+50)
    print(response)
    return Response(response,200)


if __name__ == "__main__":

    print(solar_soc())
    print(solar_net())
