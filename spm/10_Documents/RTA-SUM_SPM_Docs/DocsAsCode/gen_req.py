import os
import re

mod = "SPM"

val_1 = """
 .. comp_req:: {req}
    :id: SRS_{mod} R_{val}
    :rreqs_tag: [[add_req_info()]]
    :status: Accepted
    :source: SDV-2017 SDV 2.0 Vehicle Power Management Architecture_174 0p0
 """

val_2 = """
 .. comp_req:: {req}
    :id: SRS_{mod} R_{val}
    :rreqs_tag: [[add_req_info()]]
    :status: Accepted
    :source: SDV-2017 SDV 2.0 Vehicle Power Management Architecture_174 0p0
 """
cntr_f = re.compile(".*SRS_SPM R_([0-9]+).*")

# def append_file(value, cntr):
# 
#     with open(os.path.normpath('./tmp.rst'), 'a') as fw:
#         while(True):
#             id = input('new req: ')
#             
#             if "end" in id:
#                 break
#             
#             fw.write(value.format(req=id, val="{0:04}".format(cntr)) + "\n\n")
#             cntr += 1
# 
# def write_file(value, cntr):
#     
#     print("gen file")
# 
#     with open(os.path.normpath('./tmp.rst'), 'w') as fw:
#         while(True):
#             id = input('new req: ')
#             
#             if "end" in id:
#                 break
#             
#             fw.write(value.format(req="{0:04}".format(int(id)), val="{0:03}".format(cntr+48)) + "\n\n")
#             cntr += 1
# 
# def main():
#     
#     selector = input('select:\n\t0: SDV-2017\n\t1:SDV-8217\nSelect: ')
#     
#     if os.path.isfile(os.path.normpath('./tmp.rst')):
#         cntr = 1
#         with open('tmp.rst', 'r') as fw:
#             for l in fw.readlines():
#                 if cntr_f.match(l):
#                     cntr = int(cntr_f.match(l).group(1))
#         print("last cntr = {}".format(cntr))
# 
#         if int(selector) == 1:
#             append_file(val_2, cntr)
#             return
#         if int(selector) == 0:
#             append_file(val_1, cntr)
#             return      
#     else:
#         cntr = 1
#         print("new")
#         print("last cntr = {}".format(cntr))
# 
#         if int(selector) == 1:
#             write_file(val_2, cntr)
#             return
#         if int(selector) == 0:
#             write_file(val_1, cntr)
#             return   

def main():

    id_re = re.compile(f".*\:id\:.*SRS_{mod}.*R_([0-9]+).*")

    with open("./doc/Requirements/RTA-SUM_SPM_SRS.rst", "r", encoding='utf-8') as fr:
        reqs = []
        for l in fr.readlines():
            if id_re.match(l):
                reqs.append(int(id_re.match(l).group(1)))

    # print(reqs) 

    for i in range(1, max(reqs) + 1):
        if i % 10 == 0:
            try:
                reqs.index(i) is not None
            except ValueError:
                print("-----")
            else:
                print("R_{0:03}".format(i))
                
        else:
            try:
                reqs.index(i) is not None
            except ValueError:
                print("-----", end= " ")
            else:
                print("R_{0:03}".format(i), end= " ")
    print("\n", end="")

    print("Please fill the following form:")
    customer_id = input("\tCustomer Requirement Name (Take it from customer): ")
    print("Available customer document for SPM:\n\t0: SDV-2017\n\t1:SDV-8217\n")
    customer_doc = input("\tPlease specify customer requirement document putting a number as previous menu displays: ")

    if int(customer_doc) > 1:
        raise Exception("Invalid customer document")
   
    new_id = "{0:03}".format((sorted(reqs, reverse=True))[0] + 1)

    if int(customer_doc) == 0:
        print("New requirement:\n\n{}".format(val_1.format(req=customer_id, mod=mod, val=new_id)))
        return
    
    if int(customer_doc) == 1:
        print("New requirement:\n\n{}".format(val_2.format(req=customer_id, mod=mod, val=new_id)))
        return


if __name__ == "__main__":
    main()
