import React, { Suspense, useMemo } from 'react';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Environment, ContactShadows } from '@react-three/drei';
import Board3D from './Board3D';


export default function ThreeScene(props) {
  const { state, onTileClick, onPawnMove, selected } = props;
  const camera = useMemo(() => ({ position: [0, 2.6, 2.6], fov: 45 }), []);

  return (
    <div className="scene-wrap">
      <Canvas camera={camera} shadows>
        <ambientLight intensity={0.4} />
        <directionalLight castShadow position={[5, 8, 5]} intensity={0.7} shadow-mapSize-width={1024} shadow-mapSize-height={1024} />
        <Suspense fallback={null}>
          <Environment preset="studio" />
          <Board3D state={state} onTileClick={onTileClick} onPawnMove={onPawnMove} selected={selected} />
          <ContactShadows position={[0, -0.01, 0]} opacity={0.6} width={4} blur={1.6} far={1.6} />
        </Suspense>
        <OrbitControls makeDefault enableZoom={true} maxPolarAngle={Math.PI / 2.2} minPolarAngle={Math.PI / 4} />
      </Canvas>
    </div>
  );
}