import { FC } from 'react';
import { Navigate, Routes, Route } from 'react-router-dom';

import DemoProject from './DemoProject';
import AirCon from './AirCon';

const ProjectRouting: FC = () => {
  return (
    <Routes>
      {
        // Add the default route for your project below
      }
      <Route path="/*" element={<Navigate to="demo/information" />} />
      {
        // Add your project page routes below.
      }
      <Route path="demo/*" element={<DemoProject />} />
      <Route path="airCon/*" element={<AirCon />} />
    </Routes>
  );
};

export default ProjectRouting;
