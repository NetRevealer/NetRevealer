path="/home/abdellahxiv/data"
cd $path
for label in *
do 
cd $label
for file in *
do 
python /home/abdellahxiv/Documents/github/NetDump/tools/tcpdumpTocsv.py $file
done
mkdir CSVs
mv *.csv CSVs
rm -f *.csv
cd CSVs
for csv in *
do
cd  /home/abdellahxiv/Documents/github/NetDump
python tools/packetsToFlows.py $path/$label/CSVs/$csv /home/abdellahxiv/data.csv $label
done
cd $path
done


