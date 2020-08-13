import matplotlib.pyplot as plt

def plot_app_usage(data):
    print(data)
    try:
        packs = []
        labels = []

        data  = data.split(",")[:-1];
        for d in data:
            labels.append(d.split(":")[0])
            packs.append(int(d.split(":")[1]))
        
        tem_explode = []  # only "explode" the 2nd slice (i.e. 'Hogs')
        for l in labels:
            tem_explode.append(0.05)
        explode = tuple(tem_explode)
        fig1, ax1 = plt.subplots()
        ax1.pie(packs, explode=explode, labels=labels, autopct='%1.1f%%',
            shadow=True, startangle=90)
        ax1.axis('equal')
        plt.savefig('appusage.png')
    except Exception as e:
        print(e)
