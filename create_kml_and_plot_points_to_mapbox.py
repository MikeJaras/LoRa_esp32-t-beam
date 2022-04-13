import re
import simplekml
import geopy
from geopy import distance
import pandas as pd

from display_on_map import *

kml = simplekml.Kml()
home_lat = "44.2806"
home_lng = "16.0757"
coords_home = (home_lat, home_lng)
max_dist = 0
column_names = {"time", "lat", "long", "rssi", "distance", "colorsize"}


def check():
    # datafile = file('lora_tty.txt')
    found = False
    for line in datafile:
        if "Time: " in line:
            found = True
            break



if __name__ == '__main__':
#
# Format for data points in filename.txt
#     Time: 16:49: 20
#     Lat: 59.2775
#     Long: 18.0759
#     with RSSI - 107


    datafile = open(r"filename.txt", "r" , encoding="latin-1")
    df = pd.DataFrame(columns=column_names)
    datapoint = 0

    for line in datafile:
        error = False
        if "stop" in line:
            print("stopping")


        if "Time: " in line:
            regex = "^Time: ([0-9\:]{8})"
            try:
                time = re.search(regex, line).group()
            except:
                print("Error on line", line)
                error = True
                continue

            time_line = line
            lat_line = datafile.readline()
            lng_line = datafile.readline()

            regex = "Time: ([0-9\:]{8})"
            try:
                time = re.search(regex, time_line).group(1)
            except:
                print("Error on line", line)
                error = True
            regex = "^Lat: ([0-9\.]{7})"
            try:
                lat = re.search(regex, lat_line).group(1)
            except:
                print("Error on lat-line", line)
                error = True
            regex = "^Long: ([0-9\.]{7})"
            try:
                lng = re.search(regex, lng_line).group(1)
            except:
                print("Error on lng-line", line)
                error = True
            regex = "with RSSI (-[0-9]{2,4})"
            try:
                rssi = re.search(regex, lng_line).group(1)
            except:
                print("Error on line, no rssi", line)
                rssi = -1
                #error = True
            datapoint += 1

            if not error:
                coords_dist = (lat, lng)
                dist = geopy.distance.geodesic(coords_home, coords_dist).km
                desc = "RSSI = {} \n Distance = {:.3f}".format(rssi, dist)
                kml.newpoint(name=time, description=desc, coords=[(lng, lat)])
                if dist > max_dist:
                    max_dist = dist
                    max_time = time

#                d = {"time": time, "lat": float(lat), "long": float(lng), "rssi": [rssi], "distance": dist}
#                d = {"time": time, "lat": float(lat), "long": float(lng), "rssi": -(float(rssi)), "distance": dist}
                b = -(float(rssi))
                c = (b-70)*2.5
                a = int(c)
                d = {"time": time, "lat": float(lat), "long": float(lng), "rssi": rssi, "distance": round(dist,3), "colorsize": a }
                df.loc[datapoint] = d
                #df.append(d, ignore_index=True)
                # df = pd.DataFrame(data=d, index=[datapoint])
                pass

    # kml.save("lora_ant_II.kml")
    datafile.close()
    max_dist = "{:.3f}".format(max_dist)
    print("Max distance =", max_dist, "km", "\nTime was =", max_time)
    pass
    build_map(df)
    pass

    print()

