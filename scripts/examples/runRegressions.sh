
suffix=$(date +%Y-%m-%d-%H-%M-%S).txt

(
echo "BEGIN LIST OF REGRESSIONS $(date)"
for i in $(ls *.sh|bash listRegressions.sh)
do
	echo $i
done
echo "END^^ LIST OF REGRESSIONS $(date)"
echo ""
echo ""

for i in $(bash listRegressions.sh)
do
	bash runRegression.sh $i $suffix
done 

)|& tee Regressions-$suffix