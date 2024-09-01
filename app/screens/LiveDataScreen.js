import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { WebView } from 'react-native-webview';

const LiveDataScreen = () => {
  const [gas, setGas] = useState(null);
  const [co, setCo] = useState(null);

  useEffect(() => {
    const ws = new WebSocket('ws://192.168.1.155/ws');

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
      <Text style={styles.label}>Gas: {gas ? `${gas} ppm` : 'Loading...'}</Text>
      <Text style={styles.label}>CO: {co ? `${co} ppm` : 'Loading...'}</Text>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#fff',
  },
  label: {
    fontSize: 24,
    marginBottom: 20,
  },
});

export default LiveDataScreen;
