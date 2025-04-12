import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet, FlatList, TouchableOpacity } from 'react-native';
import axios from 'axios';
import { Ionicons } from '@expo/vector-icons';

const TableScreen = () => {
  const [gasData, setGasData] = useState([]);
  const [coData, setCoData] = useState([]);
  const [currentTable, setCurrentTable] = useState('Gas'); // 'Gas' or 'CO'
  const [currentPage, setCurrentPage] = useState(1); // Pagination

  useEffect(() => {
    fetchData(1, setGasData); // Fetch Gas data (field1)
    fetchData(2, setCoData); // Fetch CO data (field2)
  }, []);

  const fetchData = async (field, setData) => {
    try {
      const response = await axios.get(
        `https://api.thingspeak.com/channels/2572875/fields/${field}.json?api_key=EVUOCDYFKA4Q19RU&results=100`
      );
      const data = response.data.feeds.map(feed => ({
        value: feed[`field${field}`],
        timestamp: new Date(feed.created_at).toLocaleString(),
      }));
      setData(data);
    } catch (error) {
      console.error("Error fetching data:", error);
    }
  };

  const renderTable = (data, title) => {
    const itemsPerPage = 5;
    const startIndex = (currentPage - 1) * itemsPerPage;
    const endIndex = startIndex + itemsPerPage;
    const paginatedData = data.slice(startIndex, endIndex);

    return (
      <View style={styles.table}>
        <Text style={styles.tableTitle}>{title}</Text>
        <View style={styles.tableHeader}>
          <Text style={[styles.tableCell, styles.headerCell]}>Timestamp</Text>
          <Text style={[styles.tableCell, styles.headerCell]}>Value (PPM)</Text>
        </View>
        <FlatList
          data={paginatedData}
          keyExtractor={(item, index) => index.toString()}
          renderItem={({ item }) => (
            <View style={styles.tableRow}>
              <Text style={styles.tableCell}>{item.timestamp}</Text>
              <Text style={styles.tableCell}>{item.value}</Text>
            </View>
          )}
        />
      </View>
    );
  };

  return (
    <View style={styles.container}>
      <View style={styles.switchContainer}>
        <TouchableOpacity
          style={[styles.switchButton, currentTable === 'Gas' && styles.activeButton]}
          onPress={() => setCurrentTable('Gas')}
        >
          <Text style={[styles.switchButtonText, currentTable === 'Gas' && styles.activeButtonText]}>Gas Data</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={[styles.switchButton, currentTable === 'CO' && styles.activeButton]}
          onPress={() => setCurrentTable('CO')}
        >
          <Text style={[styles.switchButtonText, currentTable === 'CO' && styles.activeButtonText]}>CO Data</Text>
        </TouchableOpacity>
      </View>

      {currentTable === 'Gas'
        ? renderTable(gasData, 'Gas Sensor Data')
        : renderTable(coData, 'CO Sensor Data')}

      <View style={styles.paginationContainer}>
        <TouchableOpacity
          style={[styles.paginationButton, currentPage === 1 && styles.disabledButton]}
          onPress={() => setCurrentPage(prevPage => Math.max(prevPage - 1, 1))}
          disabled={currentPage === 1}
        >
          <Ionicons name="chevron-back" size={24} color={currentPage === 1 ? '#ccc' : '#fff'} />
        </TouchableOpacity>
        <Text style={styles.pageNumber}>Page {currentPage}</Text>
        <TouchableOpacity
          style={[styles.paginationButton, currentPage * 8 >= gasData.length && styles.disabledButton]}
          onPress={() =>
            setCurrentPage(prevPage =>
              prevPage * 8 < gasData.length ? prevPage + 1 : prevPage
            )
          }
          disabled={currentPage * 8 >= gasData.length}
        >
          <Ionicons name="chevron-forward" size={24} color={currentPage * 8 >= gasData.length ? '#ccc' : '#fff'} />
        </TouchableOpacity>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    paddingTop: 50,
    flex: 1,
    padding: 16,
    backgroundColor: '#f5f5f5',
  },
  switchContainer: {
    flexDirection: 'row',
    justifyContent: 'space-around',
    marginBottom: 20,
  },
  switchButton: {
    flex: 1,
    paddingVertical: 12,
    marginHorizontal: 5,
    backgroundColor: '#6200ee',
    borderRadius: 25,
    alignItems: 'center',
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.3,
    shadowRadius: 5,
    elevation: 5,
  },
  activeButton: {
    backgroundColor: '#3700b3',
  },
  switchButtonText: {
    color: '#fff',
    fontSize: 18,
    fontWeight: 'bold',
  },
  activeButtonText: {
    color: '#ffeb3b',
  },
  table: {
    flex: 1,
    marginBottom: 20,
  },
  tableTitle: {
    fontSize: 22,
    fontWeight: 'bold',
    marginBottom: 10,
    color: '#333',
    textAlign: 'center',
  },
  tableHeader: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    paddingVertical: 12,
    paddingHorizontal: 10,
    backgroundColor: '#e0e0e0',
    borderRadius: 10,
    marginBottom: 10,
  },
  tableRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    paddingVertical: 12,
    paddingHorizontal: 10,
    borderBottomWidth: 1,
    borderBottomColor: '#ccc',
    backgroundColor: '#fff',
    borderRadius: 10,
    marginBottom: 10,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.1,
    shadowRadius: 5,
    elevation: 3,
  },
  tableCell: {
    fontSize: 16,
    width: '50%',
    textAlign: 'center',
  },
  headerCell: {
    fontWeight: 'bold',
    fontSize: 16,
    color: '#333',
  },
  paginationContainer: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    position: 'absolute',
    bottom: 20,
    left: 16,
    right: 16,
    backgroundColor: '#fff',
    paddingVertical: 10,
    paddingHorizontal: 20,
    borderRadius: 25,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.3,
    shadowRadius: 5,
    elevation: 5,
  },
  paginationButton: {
    padding: 10,
    backgroundColor: '#6200ee',
    borderRadius: 25,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.3,
    shadowRadius: 5,
    elevation: 5,
  },
  disabledButton: {
    backgroundColor: '#ccc',
  },
  pageNumber: {
    fontSize: 16,
    color: '#333',
  },
});

export default TableScreen;
