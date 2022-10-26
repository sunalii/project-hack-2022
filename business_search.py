from selenium import webdriver
import re
from operator import itemgetter
from webinterface import WebInterface
import pycountry

def getAddressFromBusResearch(wbInt, company_name):
	if len(company_name) >= 100:
		return

	wbInt.get("https://businesssearch.sos.ca.gov/", '#LLCNameOpt')
	if "llc" in company_name.lower() or "lp" in company_name.lower():
		wbInt.getElementById('LLCNameOpt').click()
	else:
		wbInt.getElementById('CorpNameOpt').click()
	searchbox = wbInt.getElementById('SearchCriteria')
	searchbox.clear()
	searchbox.send_keys(company_name)
	wbInt.getElementByAttribute("button", "type", "submit").click()
		
	total = int(re.findall(r'[0-9]+', wbInt.getElementById("enitityTable_info", True).text)[-1])
	shown = 0
	toClickOn = None
	Flag = False
	while shown < total:
		rows = wbInt.getChildrenWithTag(wbInt.getElementByTag("tbody"), "tr", True)
		currShown = len(rows)
		rowNumber = 0
		for row in rows:
			row_cells = wbInt.getChildrenWithTag(row, "td")
			if row_cells[0].get_attribute('class') == 'dataTables_empty':
				continue
			status = row_cells[2].text.strip().lower()
			if status == 'active' and company_name.lower().replace(',', '').replace('.', '') == row_cells[3].get_attribute("data-order").lower().replace(',', '').replace('.', ''):
				toClickOn = wbInt.getChildrenWithTag(row_cells[3], "button")[0]
				Flag = True
				break
			rowNumber += 1
		if Flag:
			break
		shown += currShown
		wbInt.getChildWithTag(wbInt.getElementById("enitityTable_next"), "a").click()
	if Flag:
		toClickOn.click()
		wbInt.getElementByCSS("li.hereUmbr", wait=True)
		myElem = wbInt.getElementByAttribute("label", "for", "Address", wait=False)
		if myElem is None:
			return getAddressFromBusResearch(wbInt, company_name) # some failure happened, try again
		address = [x.strip() for x in myElem.find_element_by_xpath('..').text.split('\n')]
		citystatezip = address.pop()
		country = None
		for c in pycountry.countries:
			if c.name.upper() in citystatezip:
				country = c.name
		if not country is None:
			zip = '';
			state = country
			city = citystatezip.replace(country.upper(), '')
		else:
			zip = citystatezip.split(' ')[-1]
			state = citystatezip.split(' ')[-2]
			city = citystatezip.replace(zip, '').replace(state, '').strip()
		address.append(city)
		address.append(state)
		address.append(zip)
		address.insert(0, 'California Secretary of State')
		return address
	return None
	

def getAddressesFromBusResearch(company_names, indices, addresses):
	wbInt = WebInterface()
	lastIn = -1
		
	for i in range(len(addresses)-1, 0, -1):
		if not addresses[i] is None:
			if addresses[i][0] == 'California Business Search':
				lastIn = i;
				break;
			if addresses[i][0] in ['Google Maps', 'Cortera', 'Corporation Wiki']:
				lastIn = indices[-1]
				break
				
	for i in indices:
		if i <= lastIn: continue
		while True:
			try:
				addresses[i] = getAddressFromBusResearch(wbInt, company_names[i])
				print(addresses[i])
				break
			except:
				print("Problematic search: California Business Search, " + company_names[i] + ", Retrying")
				import traceback
				traceback.print_exc()
	wbInt.quit()
	
if __name__=="__main__":
	company_names = ['Rayotek Scientific Inc']
	addresses = [0]
	getAddressesFromBusResearch(company_names, [0], addresses)
	for company in addresses:
		print(company)





# options.binary_location = r"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe"
#     driver = webdriver.Chrome(options=options, executable_path=r'C:\\chromedriver_win32\\chromedriver.exe')