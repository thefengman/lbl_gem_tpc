
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"

void view_numSMEvents_vs_duration() {
	/*** Used only when input file is an _interpreted_Hits.root file (called Hits file). Displays a 1D histogram of Count (SM events) vs Duration of SM event (BCIDs). There's 1 histogram for the entire Hits file (many SM events). 

	Original purpose: Record the average duration of SM events, and see if it changes when I modify the clock speed.

	(Originally named view_BCIDsPerSMEvent).
	***/
	gROOT->Reset(); 

	//--Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/100_module_202_new_stop_mode_ext_trigger_scan_interpreted_Hits.root");

	if (!f) { // if we cannot open the file, print an error message and return immediately
		cout << "Error: cannot open the root file!\n";
		//return;
	}

	TTreeReader reader("Table", f);

	TTreeReaderValue<UInt_t> h5_file_num(reader, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(reader, "event_number");
	TTreeReaderValue<UChar_t> tot(reader, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(reader, "relative_BCID");
	TTreeReaderValue<Double_t> x(reader, "x");
	TTreeReaderValue<Double_t> y(reader, "y");
	TTreeReaderValue<Double_t> z(reader, "z");

	// Initialize the histogram
	TCanvas *c1 = new TCanvas("c1", "Durations of All SM Events", 1000, 10, 900, 550);
	TH1F *h = new TH1F("h", "Durations of All SM Events", 60, 0, 60);
	h->GetXaxis()->SetTitle("Duration of SM event (BCIDs)");
	h->GetYaxis()->SetTitle("Count (SM events)");
	c1->SetRightMargin(0.25);
	//h->SetMarkerStyle(7);

	// Variables used in loop
	int bcidsInCurrEvent = 0; // BCIDs in current event
	int smEventNum = -1; // the current SM-event
	int currBCID = -1; // current BCID of the SM-event (0 - 255)
	int smEventNum_f = 0; // the current SM-event FROM HELPER FUNCTION
	int currBCID_f = 0; // current BCID of the SM-event (0 - 255) FROM HELPER FUNCTION

	while (reader.Next()) {
		
		smEventNum_f = getSMEventNum(*event_number, *relative_BCID);
		currBCID_f = getSMRelBCID(*event_number, *relative_BCID);


		if (smEventNum_f == smEventNum && \
			currBCID_f > currBCID) { // its a new bcid

			bcidsInCurrEvent++;
			currBCID = currBCID_f;
		} else if (smEventNum_f > smEventNum) { // new SM-event
			h->Fill(bcidsInCurrEvent);
			smEventNum = smEventNum_f;
			currBCID = -1;
			bcidsInCurrEvent = 1;
		}
	}
	h->Fill(bcidsInCurrEvent); // fill for the last SM-event

	h->Draw("COLZ");
    c1->Update();
}