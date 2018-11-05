all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/solution/syn/report/InitiationInterval_csynth.rpt report.rpt

clean:
	rm -r hls || true
	rm report.rpt || true
	rm vivado_hls.log || true

.PHONY: all clean
