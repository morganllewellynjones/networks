R3.	Why are standard important for protocols?
A protocol loses its efficacy if it is not followed consistently and to the same standard. Interop communication breaks down if the different machines fail to follow the same standards of communication.

R10. Describe the most popular wireless Internet access technologies today. Compare and contrast them.

DSL -> ISP is your telco provider. Access through phone line which connects directly to a telnot central office, house must be within 5-10 miles of central office.

Cable -> ISP is your cable television provider. Access usually faster than DSL, but shared with many other users.

R14. Why will two ISPs at the same level of the hierarchy often peer with each other? How does an IXP earn money?

To avoid paying fee's to a higher tier ISP for intermediary traffic. IXP's primarily save money by allowing multiple ISP's to circumnavigate traffic fee's from higher tier providers. Some IXP's may charge money to lower tier ISP connectors who use the exchange point.

R16. Some content providers have created their own networks. Describe Google's network. What motivates content providers to create these networks?

To grant more control over how the content providers data is distributed. An internal network coverage also reduces traffic on higher tier ISP's and the corresponding traffic fee's.

R16. Consider sending a packet from a source host to a destination host over a fixed route. List the delay componenets in the end-to-end delay. Which of these delays are constant and which are variable?

Processing, Queuing, Transmission, and Propagation delays. Queuing delay is a factor of incoming traffic and so is variable. The other delays are a factor of the technology itself and will not vary on the same route.

R23. What are the five layers in the Internet protocol stack? What are the principle responsibilities of each of these layers?

Application -> The application code itself
Transport -> Transport between application endpoints
Network -> Transport from one machine host to another
Link -> Transport between intermediary routers
Physical -> Transport bits on the wire

Problems 6 (a-f) 10, 13, 14, 18, 21, 31.
(on problem 18 change three different hours to two different hours).

a. dprop = m/s
b. L/R
c. d-end-to-end = 2L/R + m/s
d. on the wire
e. still on the wire
f. at the destination router

P10.
d-end-to-end = 3*dproc + (L/R1 + 2L/R2 + L/R3) + (d1 / s1 + d2 / s2 + d3 / s3)

L = 1500 bytes = 12000 bits
s = 2.5 * 10^8m/s = 2.5 * 10^5 km/s = 250000 km/s
R = 2.5 Mbps = 2.5 * 2^20 bits = 2621440 bits
dproc = 3msec
d1 = 5000km
d2 = 4000km
d3 = 1000km

delay = 0.009seconds + (12000 * 4 / 2621440)seconds + (10000 / 250000)seconds = 0.009 + 0.018310546875 + 0.04 = 0.0273... seconds.

delay = 9msec + (6000 / 2560)seconds + (10000 / 250000)seconds
delay = 0.001 + 2.34375 

P13.

If N packets arrive simultaneously the queue time for each is (n-n)L/R + (n-(n-1))L/R + ... + (n-1)L/R. Total time for all is (n(n-1)/2)L/R and the average wait time for each is (n(n-1)/2n)L/R = ((n-1)/2)L/R.

P14.



P18.

P21.

P31.


