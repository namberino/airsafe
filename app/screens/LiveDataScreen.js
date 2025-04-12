import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { Ionicons } from '@expo/vector-icons';

const LiveDataScreen = () => {
  const [gas, setGas] = useState(null);
  const [co, setCo] = useState(null);

  useEffect(() => {
    const ws = new WebSocket('ws://192.168.76.212/ws');

    ws.onmessage = (event) => {
      const [gasValue, coValue] = event.data.split(',');
      setGas(gasValue);
      setCo(coValue);
    };

    ws.onerror = (error) => {
      console.error('WebSocket error:', error);
    };

    return () => {
      ws.close();
    };
  }, []);

  return (
    <View style={styles.container}>
      <Ionicons name="cloud-outline" size={64} color="#6200ee" style={styles.icon} />
      <Text style={styles.title}>Live Air Quality Data</Text>
      <Text style={styles.subtitle}>Monitor gas and CO levels in real-time.</Text>

      {/* Gas Data Container */}
      <View style={styles.dataContainer}>
        <Text style={styles.label}>Gas</Text>
        <Text style={styles.value}>{gas ? `${gas} ppm` : 'Loading...'}</Text>
      </View>

      {/* CO Data Container */}
      <View style={styles.dataContainer}>
        <Text style={styles.label}>CO</Text>
        <Text style={styles.value}>{co ? `${co} ppm` : 'Loading...'}</Text>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    paddingTop: 50,
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#f5f5f5',
    padding: 20,
  },
  icon: {
    marginBottom: 20,
  },
  title: {
    fontSize: 28,
    fontWeight: 'bold',
    color: '#333',
    marginBottom: 10,
  },
  subtitle: {
    fontSize: 18,
    color: '#666',
    marginBottom: 40,
    textAlign: 'center',
  },
  dataContainer: {
    backgroundColor: '#6200ee',
    paddingVertical: 20,
    paddingHorizontal: 30,
    borderRadius: 25,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.3,
    shadowRadius: 5,
    elevation: 5,
    alignItems: 'center',
    minWidth: '80%',
    marginBottom: 20,  // Adds spacing between the containers
  },
  label: {
    color: 'white',
    fontSize: 22,
    fontWeight: 'bold',
    marginBottom: 10,
  },
  value: {
    color: 'white',
    fontSize: 20,
  },
});

export default LiveDataScreen;
