# DNS Cache Manager (LRU Caching System)

##Overview
This C++ program implements a **DNS caching system** with **Least Recently Used (LRU)** eviction. It mimics how real DNS resolvers cache and update domain-to-IP mappings, allowing efficient lookups, automatic eviction of old entries, and synchronization with an external DNS file (`dns.txt`).

The project demonstrates file I/O operations, STL containers (`unordered_map`, `list`, `vector`), and caching strategies commonly used in system-level software.

---

##Features

- **LRU Cache Implementation**  
  Stores domain–IP pairs and automatically evicts the least recently used entry when capacity is reached.

- **File Lookup & Synchronization**  
  If a domain isn’t found in the cache, it is loaded from the `dns.txt` file.  
  Cache entries can also be synchronized with updates in this file.

- **Cache Maintenance**
  - **`clean_up_cache()`**: Removes outdated or invalid entries based on the file contents.
  - **`add_update()`**: Allows the user to add or update DNS records via console input.

- **Utility Functions**
  - `print_cache()`: Displays all cache entries.
  - `print_dns_file()`: Prints all records from the `dns.txt` file.

---

##️ Program Structure

| Function | Description |
|-----------|--------------|
| `get_ip_address_from_file()` | Reads `dns.txt` to find a domain’s IP. |
| `get_ip_address()` | Retrieves an IP from the cache or file; updates cache order. |
| `update_position()` | Marks an entry as most recently used. |
| `print_cache()` | Prints current cache state. |
| `print_dns_file()` | Displays contents of `dns.txt`. |
| `clean_up_cache()` | Removes or updates entries to match file state. |
| `add_update()` | Adds or updates DNS records interactively. |
| `main()` | Demonstrates full functionality with test domains and file updates. |

---

## LRU Caching Logic

The cache is managed using:
- A **`list<pair<string, string>>`** to maintain access order.  
  (Front = least recently used, Back = most recently used)
- An **`unordered_map<string, list::iterator>`** for constant-time lookups.

When the cache is full:
1. The least recently used entry (front of the list) is evicted.
2. The new domain–IP pair is added at the end.
3. The map is updated with the new iterator reference.

---

## File Format

The program uses a text file named `dns.txt` containing domain-to-IP mappings in the format:

```
www.google.com=8.8.8.8
www.github.com=192.30.255.113
www.example.com=192.168.1.101
```

Each line represents a domain record.

---

## How to Compile and Run

### 1. Compile
```bash
g++ -std=c++17 -o dns_cache assignment1.cpp
```

### 2. Run
```bash
./dns_cache
```

### 3. Sample Interaction
```
Enter a domain name: www.newsite.com
Enter an IP address: 123.45.67.89
New DNS record added: www.newsite.com=123.45.67.89
```

---

## Demonstration Steps in `main()`

1. Load five initial domains into the cache.
2. Access one to update its position.
3. Add a sixth domain → triggers LRU eviction.
4. Print cache before and after cleanup.
5. Update DNS file → reflect changes in cache.
6. Allow user input to modify DNS file interactively.

---

## Example Output

```
IP address for www.google.com: 8.8.8.8
Cache contents:
www.example.com = 192.168.1.101
www.google.com = 8.8.8.8
...
LRU entry removed: www.example.com
Cache updated successfully!
```

---

## Requirements

- **C++17 or later**
- `dns.txt` file in the same directory as the executable

---

## Author

**Kateri Duranceau**  
*Implements DNS cache management with LRU eviction and file synchronization.*
