#include "stdafx.h"
#ifdef USE_OPENMP
#include <omp.h>
#endif

int BuildCapacityNetwork(const TStr& InFNm, PNEANet &Net, const int& SrcColId = 0, const int& DstColId = 1, const int& CapColId = 2) {
	TSsParser Ss(InFNm, ssfWhiteSep, true, true, true);
	TRnd Random;
	Net.Clr();
	Net = TNEANet::New();
	int SrcNId, DstNId, CapVal, EId;
	int MaxCap = 0;

	CapVal = 1;

	while (Ss.Next()) {
		if (!Ss.GetInt(SrcColId, SrcNId) || !Ss.GetInt(DstColId, DstNId)) { continue; }
		//Ss.GetInt(CapColId, CapVal);
		//CapVal = Random.GetUniDevInt(1, 10000);
		//MaxCap = MAX(CapVal, MaxCap);
		if (!Net->IsNode(SrcNId)) {
			Net->AddNode(SrcNId);
		}
		if (!Net->IsNode(DstNId)) {
			Net->AddNode(DstNId);
		}
		EId = Net->AddEdge(SrcNId, DstNId);
		Net->AddIntAttrDatE(EId, CapVal, TSnap::CapAttrName);
	}
	Net->Defrag();
	return MaxCap;
}

double getcputime() {
#ifdef GLib_WIN
	return 0.0;
#else
	double result;
	struct rusage rusage;
#ifdef RUSAGE_THREAD
	getrusage(RUSAGE_THREAD, &rusage);
#else
	getrusage(RUSAGE_SELF, &rusage);
#endif
	result =
		((double)(rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000000) +
		((double)(rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec));
	return result;
#endif
}

//flows -i:small_sample2.txt -o:d:\test2.txt -src:1 -snk:5
int main(int argc, char* argv[]) {

	Env = TEnv(argc, argv, TNotify::StdNotify);
	Env.PrepArgs(TStr::Fmt("Flow. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
	double NetPRTimeSum = 0;
	double NetEKTimeSum = 0;
	int NumWins = 0;
	Try
	
		const TStr InFNm = Env.GetIfArgPrefixStr("-i:", NULL, "Input file");
	const TStr SaveOut = Env.GetIfArgPrefixStr("-o:", NULL, "Output Bipartite Flow");
	const int SrcNId = Env.GetIfArgPrefixInt("-src:", NULL, "Source Node Id");
	const int SnkNId = Env.GetIfArgPrefixInt("-snk:", NULL, "Sink Node Id");

	//const int SnkNId = 14;
	//const int SrcNId = SnkNId - 1;
	//const TStr InFNm = "D:\\SoftwareProject\\Snap-3.0\\snap_customized_cn\\flows\\12.txt";
	//const TStr InFNm = "D:\\SoftwareProject\\Snap-3.0\\snap_customized_cn\\flows\\small_sample.txt";
	//const TStr SaveOut = "D:\\SoftwareProject\\Snap-3.0\\snap_customized_cn\\flows\\12_PR.txt";

	printf("Integer Flow Test\n");
	printf("Filename: %s\n", InFNm.CStr());
	printf("Building Network...\n");
	TFIn InFile(InFNm);
	// uncomment the following lines for the binary input file
	// If the input file is a binary, use the following line to load the network
	// PNEANet Net = TNEANet::Load(InFile);
	// uncomment the following lines for the text input file
	// If the input file is a text file, use the following to load the network and save as binary
	PNEANet Net;
	BuildCapacityNetwork(InFNm, Net);
	// const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file");
	// TFOut OutFile(OutFNm);
	// Net->Save(OutFile);
	// --- calculate flows
	printf("PNEANet Nodes: %d, Edges: %d\n\n", Net->GetNodes(), Net->GetEdges());
	//#pragma omp parallel for reduction(+:NetEKTimeSum,NetPRTimeSum,NumWins) schedule(static, 1)

	TStr bip = TStr();

	//int NetMaxFlowPR = TSnap::GetMaxFlowIntPR(Net, SrcNId, SnkNId, bip);

	int NetMaxFlowEK = TSnap::GetMaxFlowIntEK(Net, SrcNId, SnkNId, bip);

	PSOut SOut = TFOut::New(SaveOut);
	bip.SaveTxt(SOut);

	//printf("\nbip: %s", bip.CStr());

	Catch

		return 0;
}

int main_old(int argc, char* argv[]) {

	/*PSOut SOut = TFOut::New("d:\\test.txt");
	TStr tt = TStr("aa12");
	tt.SaveTxt(SOut);*/


	std::string s;

	Env = TEnv(argc, argv, TNotify::StdNotify);
	Env.PrepArgs(TStr::Fmt("Flow. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
	double NetPRTimeSum = 0;
	double NetEKTimeSum = 0;
	int NumWins = 0;
	Try
		const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "small_sample.txt", "Input file");
	const int Iters = Env.GetIfArgPrefixInt("-n:", 1, "Number of runs per thread");
	const int Threads = Env.GetIfArgPrefixInt("-t:", 1, "Number of threads");
	printf("Integer Flow Test\n");
	printf("Filename: %s\n", InFNm.CStr());
	printf("Building Network...\n");
	TFIn InFile(InFNm);
	// uncomment the following lines for the binary input file
	// If the input file is a binary, use the following line to load the network
	// PNEANet Net = TNEANet::Load(InFile);
	// uncomment the following lines for the text input file
	// If the input file is a text file, use the following to load the network and save as binary
	PNEANet Net;
#if 0
	int MaxEdgeCap = BuildCapacityNetwork(InFNm, Net);
#endif
	BuildCapacityNetwork(InFNm, Net);
	// const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file");
	// TFOut OutFile(OutFNm);
	// Net->Save(OutFile);
	// --- calculate flows
	printf("PNEANet Nodes: %d, Edges: %d\n\n", Net->GetNodes(), Net->GetEdges());
#pragma omp parallel for reduction(+:NetEKTimeSum,NetPRTimeSum,NumWins) schedule(static, 1)
	for (int t = 0; t < Threads; t++) {
		TRnd Random(t);
		for (int i = 0; i < Iters; i++) {
			/*int SrcNId = Net->GetRndNId(Random);
			int SnkNId = Net->GetRndNId(Random);*/
			int SrcNId = 0;
			int SnkNId = 5;

			TStr bip = TStr();

			double PRBeginTime = getcputime();
			int NetMaxFlowPR = TSnap::GetMaxFlowIntPR(Net, SrcNId, SnkNId, bip);

			PSOut SOut = TFOut::New("d:\\test.txt");

			bip.SaveTxt(SOut);

			printf("\nbip: %s", bip.CStr());

			double PREndTime = getcputime();
			double NetPRFlowRunTime = PREndTime - PRBeginTime;

			double EKBeginTime = getcputime();
			int NetMaxFlowEK = TSnap::GetMaxFlowIntEK(Net, SrcNId, SnkNId, bip);

			double EKEndTime = getcputime();
			double NetEKFlowRunTime = EKEndTime - EKBeginTime;

			IAssert(NetMaxFlowPR == NetMaxFlowEK);

			if (NetPRFlowRunTime < NetEKFlowRunTime) { NumWins++; }

			NetPRTimeSum += NetPRFlowRunTime;
			NetEKTimeSum += NetEKFlowRunTime;

#pragma omp critical
			{
#ifdef USE_OPENMP
				printf("Thread: %d\n", omp_get_thread_num());
#endif
				printf("Source: %d, Sink %d\n", SrcNId, SnkNId);
				printf("Max Flow: %d\n", NetMaxFlowEK);
				printf("PR CPU Time: %f\n", NetPRFlowRunTime);
				printf("EK CPU Time: %f\n", NetEKFlowRunTime);
				printf("\n");
			}
		}
	}
	int TotalRuns = Iters*Threads;
	printf("Avg PR PNEANet Time: %f\n", NetPRTimeSum / TotalRuns);
	printf("Avg EK PNEANet Time: %f\n", NetEKTimeSum / TotalRuns);
	printf("%d out of %d PR was faster\n", NumWins, TotalRuns);

	Catch

		return 0;
}

