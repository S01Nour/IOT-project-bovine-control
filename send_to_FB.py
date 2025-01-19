# ---------------------------------------------------------
# firebase_send.py
# ---------------------------------------------------------
import requests
import json

def send_result_to_firebase(cow_id, is_sick, firebase_url, auth_key):
    """
    Sends the classification result to Firebase Realtime Database using a REST call.
    :param cow_id: The ID of the cow.
    :param is_sick: Boolean or integer label for sickness.
    :param firebase_url: Your base Firebase DB URL, e.g. 'https://YOUR_APP.firebaseio.com'
    :param auth_key: Your DB secret or auth token (if needed).
    """
    # For Realtime DB, typically we do: /some_collection.json?auth=AUTH_KEY
    # Here we'll store under 'Cows/cow_<cow_id>.json'
    endpoint = f"{firebase_url}/Cows/cow_{cow_id}.json?auth={auth_key}"

    data = {
        "ID": cow_id,
        "healthStatus" : "SICK" if is_sick else "HEALTHY"
    }

    try:
        response = requests.patch(endpoint, data=json.dumps(data))
        if response.status_code == 200:
            print(f"[OK] Sent to Firebase => Cow ID: {cow_id}, is_sick={is_sick}")
        else:
            print(f"[Error] Firebase responded with status {response.status_code}", response.text)
    except Exception as e:
        print("[Error] Exception while sending to Firebase:", e)

# ---------------------------------------------------------
# Example usage:
# ---------------------------------------------------------
if __name__ == "__main__":
    # Replace with your real DB URL and auth key
    my_firebase_url = "https://bovine-health-default-rtdb.europe-west1.firebasedatabase.app/"
    my_auth_key     = "AIzaSyAxY_fxzCxo-cSZHCOj7pLvJS8U1CO2gxo"

    # Suppose we have a result from the model
    cow_id = 10
    is_sick = True  # or False

    send_result_to_firebase(cow_id, is_sick, my_firebase_url, my_auth_key)
