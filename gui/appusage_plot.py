# import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

def plot_app_usage(data):
    # print(data)
    try:
        packs = []
        traffic_size = []
        labels = []

        data  = data.split(",")[:-1];
        for d in data:
            l, p, s = d.split(":")
            labels.append(l)
            packs.append(int(p))
            traffic_size.append(int(s))
        
        # print(labels)
        # print(packs)
        # print(t)
        tem_explode = []  # only "explode" the 2nd slice (i.e. 'Hogs')
        for l in labels:
            tem_explode.append(0.05)
        explode = tuple(tem_explode)
        fig, ax = plt.subplots(nrows=1, ncols=2, figsize=(13,4))
        ax[0].set_title('Application usage (Number of packets) ')
        ax[0].pie(packs, explode=explode, labels=labels, autopct='%1.1f%%',
            shadow=True, startangle=90)
        ax[0].axis('equal')

        ax[1].bar(labels, traffic_size)
        ax[1].set_title('Application usage: Traffic size (KB)')
        ax[1].set_ylabel('Traffic size (KB)')
        plt.savefig('.appusage.png')
        
        # df = pd.DataFrame({'packs': packs}, index=labels)
        # plot = df.plot.pie(y='packs', figsize=(5, 5))
        # fig = plot.get_figure()
        # fig.savefig("appusage.png")
    except Exception as e:
        print(e)
