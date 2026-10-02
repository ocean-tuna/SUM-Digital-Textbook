@page introduction_id 1. Introduction
@tableofcontents

@section Goals 1.1 Goals
The main goals of this documents are :

* Define the module RTA-SUM SSFM from a code point of view
* Map each element of the Architectural Document to code entities
* Describe in details the code entities, logic and algorithms
* Give a guideline and a base for the future changes of the module


@section definitons 1.2 Definitions, Acronyms and Abbreviations 

|Term/Abbreviation | Definition |
|:----------------:|:-----------|
| API | Application Program Interface |
| BSW | Basic Software |
| BswM | Basic Software Mode Manager |
| CAN | Controller Area Network |
| Dcm | Diagnostic Communication Module |
| Dem | Diagnostic Event Manager |
| DID | Data IDentifier |
| DTC | Diagnostic Trouble Code |
| DIAG | Diagnostic Application Services |
| ECU | Electronic Control Unit |
| EcuM | ECU State Manager |
| ERRH | Error Handler |
| LIN | Local Interconnect Network |
| LNM | LIN Network Manager |
| NVRAM | Non Volatile Random Access Memory |
| NvM | NVRAM Manager |
| OS | Operating System |
| OAW | Open Architecture Ware |
| PMS | Power Mode Slave |
| PN | Partial Network |
| PNC | Partial Network Coordinator |
| RTE | Run Time Environment |
| SOH | State Of Health Manager |
| SSM | Signal Status Monitor |
| SSFM | Signal Status and Fault Monitor |
| SSC | Security Service Coordinator |
| SUM | Standard Utility Module |
| SUSD | Startup Shut down Coordinator |
| SW | Software |
| SW-C | Software Component |
| TCS | Timing Client Service |
| WdgM | Watchdog Manager |

@section refdoc 1.3 Referenced Documents

|Document Name|Description|Version|Released|
|-------------|:---------:|:-----:|:------:|
|SDV-8002 AUTOSAR Standard Utility Modules Specification | SSFM module specification | 27.27.183.0p2 | 10/10/2025
|GB9100 Serial Data Transfer Safety Requirements | ARC, PV, CRC, CS Specification | 4.0S | 02/02/2016
|SDV-6001 Diagnostic Infrastructure Specification | Loss of Communication based on Gateway | 27.27.183.0p0 | 9/24/2025
|GB6003 Diagnostic Software Implementation Specification | ERRH UDS services | V3.4-24.24.157 | 30/10/2020