import asyncio
#uses bleak python bluetooth library, must be installed
from bleak import BleakScanner
from bleak import BleakClient
#Pico address:
addr = "2C:CF:67:E0:8A:CC"
Characteristic = "3727f058-69dd-4d78-9b6f-9c2ebbb67d44"
async def main(address):
    # devices = await BleakScanner.discover()
    # for d in devices:
        # print(d)

    async with BleakClient(address) as client:
        print(type(client))
        services = client.services.services.values()
        for item in services:
            print(f"Item: {item.characteristics}")

            for characteristic in item.characteristics:
                print(f"Characteristic: {characteristic}")
                char_description = characteristic.description
                char_uuid = characteristic.uuid
                print(f"Characteristic Description is: {char_description}")
                print(f"Characteristic UUID is: {char_uuid}")

                value = await client.read_gatt_char(char_uuid)
                print(f"Value: {value}")
        print(services)
       
asyncio.run(main(addr))