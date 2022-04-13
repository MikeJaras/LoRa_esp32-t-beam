import plotly.express as px
# token from mapbox is stored in file .mapbox_token
px.set_mapbox_access_token(open(".mapbox_token").read())
# df = px.data.carshare()
# fig = px.scatter_mapbox(df, lat="centroid_lat", lon="centroid_lon",     color="peak_hour", size="car_hours",
#                  color_continuous_scale=px.colors.cyclical.IceFire, size_max=15, zoom=10)

def build_map(df):
#    fig = px.scatter_mapbox(df, lat="lat", lon="long", size="rssi", color="distance", color_continuous_scale=px.colors.cyclical.IceFire, size_max=15, zoom=16)
#    fig = px.scatter_mapbox(df, lat="lat", lon="long", color="rssi", size="distance", color_continuous_scale=px.colors.cyclical.IceFire, size_max=15, zoom=16)
    fig = px.scatter_mapbox(df, lat="lat", lon="long", size="colorsize", color="distance",
                            color_continuous_scale=px.colors.colorbrewer.Spectral, size_max=15, zoom=16,
                            hover_data=["time","rssi", "distance"],hover_name="rssi"
                            , labels="rssi")
    fig.show()