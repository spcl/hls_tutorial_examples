## Example 7

As a final step for scaling up performance, this example restructures the code from example 6 to a linear array of processing elements. This means that when scaling up `D`, we maintain a constant fan-out within each processing element, instead forwarding each loaded value to subsequent processing elements to gain more parallelism through reuse.
This change adds one more layer of complexity: namely the forwarding of data between elements. Rather than storing `D` elements from `A` within an element, we forward all received elements but the last, which is kept as the local buffered element. Similarly, result values of `C` must be propagated through the element and written out at the beginning or end.Both of these functionalites requires loop bounds to depend on the position of the processing element in the chain
(`d`). 

This example primarily exists as a reference of how to scale up computational kernels while maintaining a constant fan-out. To see the effect of scaling up the design, following steps similar to those proposed for example 6. 
