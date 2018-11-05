all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/example1/syn/report/Simple1DStencil_csynth.rpt report.rpt

synthesis_pipelined:
	vivado_hls -f SynthesisPipelined.tcl
	cp hls_pipelined/example1/syn/report/Simple1DStencil_csynth.rpt report_pipelined.rpt

clean:
	rm -r hls || true
	rm -r hls_pipelined || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm report_pipelined.rpt || true
	
.PHONY: all clean

