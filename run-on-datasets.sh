#!/bin/bash

currentPlace=$(pwd)

function runTest {
	echo ""
	echo "***************"
	nproc=31
	date
	testName=$1
	dataDirectory=$2
	rayTemplate=$3
	reference=$4
	echo "Running" $testName
	echo "Directory=$dataDirectory"
	cd $dataDirectory
	echo "Input (from $rayTemplate):"
	cat $rayTemplate
	touch Log
	mpirun -tag-output  -np $nproc /home/boiseb01/Ray/trunk/Ray $rayTemplate > Log
	mv Log $currentPlace/$testName.Log
	/home/boiseb01/Ray/trunk/mummer-validate.rb $reference Contigs.fasta  L
	touch L
	echo "Validating with $reference"
	mv L $currentPlace/$testName.Validation
	mv CoverageDistribution.txt $currentPlace/$testName.CoverageDistribution
	mv Parameters.txt $currentPlace/$testName.Parameters
	mv Contigs.fasta $currentPlace/$testName.Contigs
}

runTest Sp /data/users/boiseb01/PaperDatasets/Sp input /home/boiseb01/nuccore/Streptococcus-pneumoniae-R6.fasta
runTest SpEr /data/users/boiseb01/PaperDatasets/SpEr input /home/boiseb01/nuccore/Streptococcus-pneumoniae-R6.fasta
runTest SpP /data/users/boiseb01/PaperDatasets/SpPaired input /home/boiseb01/nuccore/Streptococcus-pneumoniae-R6.fasta
runTest E400 /data/users/boiseb01/PaperDatasets/Ecoli400 input /home/boiseb01/nuccore/Ecoli-k12-mg1655.fasta
runTest PAO1-p /data/users/boiseb01/PaperDatasets/Pseudo input /home/boiseb01/nuccore/Pseudomonas-aeruginosa-PAO1,-complete-genome.fasta
runTest PAO1-np /data/users/boiseb01/PaperDatasets/Pseudo input-np /home/boiseb01/nuccore/Pseudomonas-aeruginosa-PAO1,-complete-genome.fasta
runTest PAO1-p1 /data/users/boiseb01/PaperDatasets/PseudoEr1 input /home/boiseb01/nuccore/Pseudomonas-aeruginosa-PAO1,-complete-genome.fasta

runTest Ep-Illumina /data/users/boiseb01/PaperDatasets/SRA001125 input /home/boiseb01/nuccore/Ecoli-k12-mg1655.fasta
runTest Enp-Illumina /data/users/boiseb01/PaperDatasets/SRA001125 input-notpaired /home/boiseb01/nuccore/Ecoli-k12-mg1655.fasta
runTest adp1-roche /data/users/boiseb01/PaperDatasets/SRA003611 roche.Ray /home/boiseb01/nuccore/adp1.fasta
runTest adp1-Illumina /data/users/boiseb01/PaperDatasets/SRA003611 Illumina.ray /home/boiseb01/nuccore/adp1.fasta
runTest adp1-mix /data/users/boiseb01/PaperDatasets/SRA003611 template.ray /home/boiseb01/nuccore/adp1.fasta

