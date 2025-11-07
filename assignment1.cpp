#include<iostream>
#include<string>
#include<fstream>
#include<unordered_map>
#include<list>
using namespace std;

// List for the access order, from least recently used --> most recently used in the back
list<pair<string, string> > cacheList;  
unordered_map<string, list<pair<string, string> >::iterator> dnsMap; // Maps domain to iterator that keeps track of when an entry is accessed
const int max_cache_size = 5;

//Get DNS information for a specific domain

string get_ip_address_from_file(const string& domain_name){
/*  
    Input: domain name 
    Output: corresponding IP address
*/  
    //open DNS file in read mode
    ifstream file("dns.txt");
    //check if successfully opened, if not, return an empty string
    if (file.fail()){
        return "";
    }
    //read the file line by line. If the key not found, return an empty string and indicate that the file couldn't be found.
    string line;
    while(getline(file, line)){
        //find the index of the delimiter then split the string there
        int delimitor_pos = line.find('=');
        if (delimitor_pos != string::npos){
            string key = line.substr(0, delimitor_pos); //get the domain name before '='
            string value = line.substr(delimitor_pos+1); //get the IP adress
            if (key == domain_name){
                file.close();
                return value;   //domain found, return the IP adress
            }
        }
    }
    file.close();
    return "";  //domain not found
}

//Function to change the access order of the different entries, in our case, moving the most recently accessed element to the end (that applies to newly added elements too)
void update_position(list<pair<string, string> >::iterator it){
    /*
        input: An itterator pointing to a specific element in the cache that is beeing accessed.
        Marks this element as the most recently accessed one, places it at the end of the list.
    */
     // Store the domain and IP, * to dereference the element the iterator points to
    pair<string, string> entry_copy = *it;
    // Remove the entry from its current position
    cacheList.erase(it);
    // Reinsert at the end 
    cacheList.push_back(entry_copy);
    // Update the iterator in the map. the map associates a domain name with an iterator pointing to an entry in our cache list.
    //--cacheList.end() points to the last valid element.
    //results in the most recent entry, the first position iterator in dnsMAP to point to the last element of the cache list.
    dnsMap[entry_copy.first] = --cacheList.end();

}

// Improving performance with caching, and a cache eviction mechanism
string get_ip_address(const string& domain_name){
/*
    Input: domain name 
    Output: corresponding IP address
*/
    //check if the ip address is found in the cache, if found, 'it' is an itterator pointing to the cache list
    unordered_map<string, list<pair<string, string> >::iterator>::iterator it = dnsMap.find(domain_name);
    if (it != dnsMap.end()){    //iterator found the domain
        string cached_ip = it->second->second;  // Store IP before moving
        update_position(it->second);    // Move it to most recently used position
        return cached_ip;      //return the associated ip address
    }
    //domain name not found in the cache, load from file
    string ip = get_ip_address_from_file(domain_name);
    //if ip address not found in the file, add it to the cache for future lookup. 
    if (!ip.empty()){
        //verify that the cache is not full, if so, delete the least used element (the first of the list)
        if(cacheList.size() >= max_cache_size){
            list<pair<string, string> >::iterator lru_entry = cacheList.begin();    //get the first entry of the list
            dnsMap.erase(lru_entry->first);         // Remove from map
            cacheList.pop_front();                 //remove from list
        }
        cacheList.push_back(make_pair(domain_name, ip));// Add the most recent entry at back 
        dnsMap[domain_name] =  --cacheList.end();  // Store iterator to last element
    }
    return ip;
}

//utility functions
void print_cache(void){
    //verify that cache isn't empty
    if (cacheList.empty()){
        cout << "The cache is empty" << endl;
        return;
    }
    //print every key: value pair of the cache
    for (const pair<string, string>& entry : cacheList) {
    cout << entry.first << " = " << entry.second << endl;  
    }
}

void print_dns_file(string filename){
    //open DNS file in read mode
    ifstream file(filename);
    if (file.fail()){
        cerr << "Error: could not load and read the file." << endl;
        return;
    }
    //read the file line by line and print.
    string line;
    while(getline(file, line)){
        cout << line << endl;
    }
    file.close(); 
}

// cache cleanup
void clean_up_cache(const string& filename) {
    // Load the DNS file into a map for comparison.
    unordered_map<string, string> fileEntries;
    ifstream file(filename);

    //verify that the file can be opened
    if (!file.is_open()) {
        cerr << "ERROR: Could not open the DNS file." << endl;
        return;
    }
    //read line by line and add to the comparison map
    string line;

    while (getline(file, line)) {
        int delimiter_pos = line.find('=');
        if (delimiter_pos != string::npos) {
            string domain = line.substr(0, delimiter_pos);
            string ip = line.substr(delimiter_pos + 1);
            fileEntries[domain] = ip;  // Store domain : IP mapping
        }
    }
    file.close();  // Close file after reading
    //  Iterate through the cache to remove or update entries
    for (list<pair<string, string> >::iterator it = cacheList.begin(); it != cacheList.end();) {
        string domain = it->first;
        string cached_ip = it->second;
        unordered_map<string, string>::iterator file_it = fileEntries.find(domain);  // Lookup domain in file map
        
        if (file_it == fileEntries.end()) {  
            //Domain not in the file, so we remove from cache
            unordered_map<string, list<pair<string, string> >::iterator>::iterator map_it = dnsMap.find(domain);
            if (map_it != dnsMap.end()) {
                dnsMap.erase(map_it);  // Remove from dnsMap
            }
            it = cacheList.erase(it);  // Remove from cacheList 
        } 
        else if (file_it->second != cached_ip) {  
            // IP address hanged, update
            it->second = file_it->second;  // Update IP in cache
            unordered_map<string, list<pair<string, string> >::iterator>::iterator map_it = dnsMap.find(domain);
            if (map_it != dnsMap.end()) {
                update_position(map_it->second);  // Move updated entry to the most recently visited domain
            }
            ++it;
        } 
        else {
            ++it;  
        }
    }
}


// add_update to cache
void add_update(const string& filename) {
    string domain, ip;
    // Ask user input, and get the domain name
    cout << "Enter a domain name: ";
    cout.flush();
    getline(cin, domain);
    // Ask user input, and get the ip address
    cout << "Enter an IP address: ";
    cout.flush();
    getline(cin, ip);

    // Check if the domain already exists in the file
    string ip_found = get_ip_address_from_file(domain);
    if (ip_found.empty()) {
        // New entry, so append to file, open the file in append mode
        ofstream outFile(filename, ios::app);
        if (!outFile.is_open()) {
            cerr << "Error: Could not open the DNS file for writing." << endl;
            return;
        }
        outFile << domain << "=" << ip << endl;
        outFile.close();
        cout << "New DNS record added: " << domain << "=" << ip << endl;
    } 
    else if (ip_found != ip) {
        // Existing entry, but different IP, update dns
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cerr << "Error: Could not open the DNS file for reading." << endl;
            return;
        }

        // Read existing records while updating the domain
        vector<pair<string, string> > dnsFileEntries;
        string line;
        bool updated = false;

        while (getline(inFile, line)) {
            int delimiter_pos = line.find('=');
            if (delimiter_pos != string::npos) {
                string file_domain = line.substr(0, delimiter_pos);
                string file_ip = line.substr(delimiter_pos + 1);
                
                if (file_domain == domain) {
                    dnsFileEntries.emplace_back(domain, ip);  //Update domain's IP
                    updated = true;
                } else {
                    dnsFileEntries.emplace_back(file_domain, file_ip);
                }
            }
        }
        inFile.close();

        // rewrite the file with updated records
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error: Could not open the DNS file for writing." << endl;
            return;
        }
        for (const pair<string, string>& entry : dnsFileEntries) {
            outFile << entry.first << "=" << entry.second << endl;
        }
        outFile.close();
        cout << "DNS record updated: " << domain << "=" << ip << endl;
    } 
    else {
        // The record already exists with the same IP
        cout << "No changes made. The DNS record is already up-to-date." << endl;
    }

    // update Cache
    unordered_map<string, list<pair<string, string> >::iterator>::iterator cache_it = dnsMap.find(domain);
    if (cache_it != dnsMap.end()) {
        // Domain exists in cache, Update it
        cache_it->second->second = ip;
        update_position(cache_it->second);
    } 
    else {
        // Domain is not in cache, add
        if (cacheList.size() >= max_cache_size) {
            // Remove least recently used entry
            list<pair<string, string> >::iterator lru_entry = cacheList.begin();
            dnsMap.erase(lru_entry->first);
            cacheList.pop_front();
        }
        // Add new entry at the end (most recent)
        cacheList.push_back(make_pair(domain, ip));
        dnsMap[domain] = --cacheList.end();
    }
}

int main() {
    // Fill the cache to its maximum capacity
    std::cout << "IP address for www.example.com: " <<
    get_ip_address("www.example.com") << std::endl;
    std::cout << "IP address for www.google.com: " <<
    get_ip_address("www.google.com") << std::endl;
    std::cout << "IP address for www.github.com: " <<
    get_ip_address("www.github.com") << std::endl;
    std::cout << "IP address for www.stackoverflow.com: " <<
    get_ip_address("www.stackoverflow.com") << std::endl;
    std::cout << "IP address for www.reddit.com: " <<
    get_ip_address("www.reddit.com") << std::endl;
    // Print the cache contents
    print_cache();
    // Access an existing entry to update its position in the LRU list
    std::cout << "IP address for www.google.com: " <<
    get_ip_address("www.google.com") << std::endl;
    // Add a new entry, which should evict the least recently used entry
    std::cout << "IP address for www.twitter.com: " <<
    get_ip_address("www.twitter.com") << std::endl;
    // Print the cache contents again to verify the eviction
    print_cache();
    // Modify the DNS file to simulate changes
    std::ofstream dnsFile("dns.txt", std::ios::trunc);
    dnsFile << "www.example.com=192.168.1.101" << std::endl;
    dnsFile << "www.google.com=8.8.8.8" << std::endl;
    dnsFile << "www.github.com=192.30.255.113" << std::endl;
    dnsFile << "www.stackoverflow.com=151.101.1.69" << std::endl;
    dnsFile << "www.reddit.com=151.101.1.70" << std::endl;
    dnsFile.close();
    // Clean up the cache
    clean_up_cache("dns.txt");
    // Print the cache contents again to verify the cleanup
    print_cache();
    // Print the contents of the DNS file
    print_dns_file("dns.txt");
    // Test the add_update function
    add_update("dns.txt");
    // Print the cache and DNS file contents again to verify the changes
    print_cache();
    std::cout << "final file " << endl;
    print_dns_file("dns.txt");
    return 0;
}

