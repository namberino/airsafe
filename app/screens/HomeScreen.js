import React from 'react';
import { View, Text, Button, StyleSheet, TouchableOpacity } from 'react-native';
import { Ionicons } from '@expo/vector-icons';
import { useNavigation } from '@react-navigation/native';

export default function HomeScreen() {
  const navigation = useNavigation();

  return (
    <View style={styles.container}>
      {/* Title */}
      <Text style={styles.title}>Welcome to the Airsafe App</Text>
      <Text style={styles.subtitle}>Monitor and analyze air quality data in real-time.</Text>
      
      {/* Buttons with Icons */}
      <TouchableOpacity style={styles.button} onPress={() => navigation.navigate('Tables')}>
        <Ionicons name="grid-outline" size={24} color="white" />
        <Text style={styles.buttonText}>View Tables</Text>
      </TouchableOpacity>
      
      <TouchableOpacity style={styles.button} onPress={() => navigation.navigate('LiveData')}>
        <Ionicons name="pulse-outline" size={24} color="white" />
        <Text style={styles.buttonText}>Live Data</Text>
      </TouchableOpacity>
      
      {/* Additional Description */}
      <Text style={styles.description}>
        Easily switch between viewing comprehensive data tables or monitoring live air quality metrics.
      </Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#f5f5f5',
    padding: 20,
  },
  menuButton: {
    position: 'absolute',
    top: 40,
    left: 20,
  },
  title: {
    fontSize: 22,
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
  button: {
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: '#6200ee',
    paddingVertical: 15,
    paddingHorizontal: 25,
    borderRadius: 25,
    marginBottom: 20,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.3,
    shadowRadius: 5,
    elevation: 5,
  },
  buttonText: {
    color: 'white',
    fontSize: 18,
    marginLeft: 10,
  },
  description: {
    fontSize: 16,
    color: '#777',
    textAlign: 'center',
    marginTop: 30,
    lineHeight: 22,
  },
});

