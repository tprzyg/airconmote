
import React, { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useRouterTab, useLayoutTitle } from '../components';

import AirConInformation from './AirConInformation';
import AirConStateRestForm from './AirConStateRestForm';
import AirConMqttSettingsForm from './AirConMqttSettingsForm';
import AirConStateWebSocketForm from './AirConStateWebSocketForm';

const AirConProject: FC = () => {
  useLayoutTitle("AirCon Project");
  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="information" label="Information" />
        <Tab value="rest" label="REST Example" />
        <Tab value="socket" label="WebSocket Example" />
        <Tab value="mqtt" label="MQTT Settings" />
      </RouterTabs>
      <Routes>
        <Route path="information" element={<AirConInformation />} />
        <Route path="rest" element={<AirConStateRestForm />} />
        <Route path="mqtt" element={<AirConMqttSettingsForm />} />
        <Route path="socket" element={<AirConStateWebSocketForm />} />
        <Route path="/*" element={<Navigate replace to="information" />} />
      </Routes>
    </>
  );
};

export default AirConProject;