all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl && echo ""
	cp hls/example1/syn/report/Simple1DStencil_csynth.rpt report.rpt

synthesis_solution:
	vivado_hls -f SynthesisSolution.tcl && echo ""
	cp hls_solution/example1/syn/report/Simple1DStencil_csynth.rpt report_solution.rpt

clean:
	rm -r hls || true
	rm -r hls_solution || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm report_solution.rpt || true
	
.PHONY: all clean

