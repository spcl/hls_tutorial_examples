all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl && echo ""
	cp hls/solution/syn/report/InitiationInterval_csynth.rpt report.rpt
