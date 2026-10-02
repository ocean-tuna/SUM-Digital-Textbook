import os
import re
import PyPDF2
from enum import Enum

rst_req_tmp = """
.. req:: {text}
    :id: [$SRS_SPM {id}]
    :status: Accepted 
    :category: Functional
    :sub_category: TO-DO                 
    :source_id: {source}           
    :test_technique: Integration Testing           
    :comments: 
    :source_doc: SDV-2017 174 0p0
    :doc_version: TODO_DOC_VERSION
    :last_update: Dec 13, 2023

    TEXT

"""

class Req(Enum) :
    OLD = 0
    NEW = 1

class Counter :
    def __init__( self, init_counter : Req ) :
        self.__counter = init_counter
        self.__req_lst = []
    
    def update_counter( self ) :
        self.__counter += 1

    def get_counter( self ) -> int :
        return self.__counter
    
    def add_req_lst( self, tag ) :
        self.__req_lst.append({"Tag" : f'SDVA-{tag[0]:003}', "Text" : tag[1].replace(f"\xa0", "") if len(tag[1]) > 0 else "No Title", "Id" : f"{self.get_counter():04}"})

    def print_obj(self) :
        with open("./RTA-SUM_SPM_Docs/DocsAsCode/doc/Requirements/10_Requirements.rst", 'w') as fw :
            fw.write("******************\nSPM Requirements\n******************\n\n")
            for req in self.__req_lst :
                fw.write(rst_req_tmp.format(text=req["Text"] if len(req["Text"]) > 0 else "", id=req["Id"], source=req["Tag"]))

def linear_list(reqs, counter : Counter) :

    if not isinstance(reqs, list) :
        counter.update_counter()
        counter.add_req_lst(reqs)
        return 
    
    for req in reqs :
        linear_list(req, counter)

def load_and_search_in_pdf( filename : str, lst_id : int ) :
    global reqs_old
    global reqs_new


    # Open the PDF file
    with open(filename, 'rb') as file:
        # Create a PDF reader object
        pdf_reader = PyPDF2.PdfReader(file)

        # Get the number of pages in the PDF
        num_pages = len(pdf_reader.pages)

        counter = 0

        # Iterate through each page and search for the string
        for page_num in range(num_pages):
            text = pdf_reader.pages[page_num].extract_text()

            found = re.findall(".*SDVA\-(\d+)\s?\:?\s?\-*(.*\n?.*)OPEN.*", text)
            #if len(found) > 0:
            for f_itm in found :
                if lst_id == Req.NEW:
                    reqs_new.append(f_itm[0])
                if lst_id == Req.OLD :
                    reqs_old.append(f_itm[0])

    # counter = Counter(0)

    # if lst_id == Req.NEW:
    #     linear_list(reqs_new, counter)
    # 
    # if lst_id == Req.OLD :
    #     linear_list(reqs_old, counter)
# 
    # counter.print_obj()    

def main() :
    global reqs_old
    global reqs_new

    reqs_old = []
    reqs_new = []

    pth_1 = os.path.normpath(r"C:\Users\pig1tn\Documents\RTA-SUM\00_System_Reqs_GM\VIP27\SDV-2017 SDV 2.0 Vehicle Power Management Architecture_174 0p0.pdf")
    pth_2 = os.path.normpath(r"C:/Users/pig1tn/Downloads/SDV-2017 - SDV 2.0 Vehicle Power Management Architecture_27.177.2p0.pdf")
    
    load_and_search_in_pdf(os.path.normpath(pth_1), Req.OLD)
    load_and_search_in_pdf(os.path.normpath(pth_2), Req.NEW)

    print(f"Old:\n" + '\n'.join(reqs_old))
    print(f"New: {reqs_new}")

    s1 = set(reqs_old)
    s2 = set(reqs_new)

    print(f"Old: {s1.difference(s2)}")
    print(f"New: {s2.difference(s1)}")

if __name__ == "__main__" :
    main()