import React from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import TableScreen from './screens/TableScreen';
import LiveDataScreen from './screens/LiveDataScreen';
import HomeScreen from './screens/HomeScreen';

const Stack = createStackNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator initialRouteName="HomeScreen">
        <Stack.Screen name="HomeScreen" component={HomeScreen} />
        <Stack.Screen name="Tables" component={TableScreen} />
        <Stack.Screen name="LiveData" component={LiveDataScreen} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}
