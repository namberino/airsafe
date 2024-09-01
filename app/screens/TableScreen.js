import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet, FlatList, Button } from 'react-native';
import axios from 'axios';

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
    const itemsPerPage = 8;
    const startIndex = (currentPage - 1) * itemsPerPage;
    const endIndex = startIndex + itemsPerPage;
    const paginatedData = data.slice(startIndex, endIndex);

    return (
      <View style={styles.table}>
        <Text style={styles.tableTitle}>{title}</Text>
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
        <View style={styles.pagination}>
          <Button
            title="Previous"
            onPress={() => setCurrentPage(prevPage => Math.max(prevPage - 1, 1))}
            disabled={currentPage === 1}
          />
          <Text style={styles.pageNumber}>Page {currentPage}</Text>
          <Button
            title="Next"
            onPress={() =>
              setCurrentPage(prevPage =>
                prevPage * itemsPerPage < data.length ? prevPage + 1 : prevPage
              )
            }
            disabled={endIndex >= data.length}
          />
        </View>
      </View>
    );
  };

  return (
    <View style={styles.container}>
      <View style={styles.switchContainer}>
        <Button
          title="Gas Data"
          onPress={() => setCurrentTable('Gas')}
          disabled={currentTable === 'Gas'}
        />
        <Button
          title="CO Data"
          onPress={() => setCurrentTable('CO')}
          disabled={currentTable === 'CO'}
        />
      </View>
      {currentTable === 'Gas'
        ? renderTable(gasData, 'Gas Sensor Data')
        : renderTable(coData, 'CO Sensor Data')}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 16,
    backgroundColor: '#fff',
  },
  switchContainer: {
    flexDirection: 'row',
    justifyContent: 'space-around',
    marginBottom: 20,
  },
  table: {
    marginBottom: 20,
  },
  tableTitle: {
    fontSize: 18,
    fontWeight: 'bold',
    marginBottom: 10,
    textAlign: 'center',
  },
  tableRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    paddingVertical: 8,
    borderBottomWidth: 1,
    borderBottomColor: '#ccc',
  },
  tableCell: {
    fontSize: 16,
    width: '50%',
    textAlign: 'center',
  },
  pagination: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginTop: 10,
  },
  pageNumber: {
    fontSize: 16,
  },
});

export default TableScreen;
