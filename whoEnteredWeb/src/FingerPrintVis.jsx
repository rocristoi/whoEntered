import React, { useEffect, useRef } from 'react';

const FingerprintVisualization = ({ fingerprintTemplate }) => {
    const canvasRef = useRef(null);


  useEffect(() => {
    // Convert hex to bytes
    const templateBytes = new Uint8Array(fingerprintTemplate.match(/.{1,2}/g).map(byte => parseInt(byte, 16)));

    // Example logic to interpret and extract minutiae points (simplified)
    const minutiaePoints = [];
    for (let i = 10; i < templateBytes.length; i += 2) {
      const x = templateBytes[i]; // 1 byte for x-coordinate
      const y = templateBytes[i + 1]; // 1 byte for y-coordinate
      minutiaePoints.push({ x, y });
    }

    // Draw minutiae points on canvas
    const canvas = canvasRef.current;
    const ctx = canvas.getContext('2d');
    const imageWidth = 256;
    const imageHeight = 288;
    
    ctx.clearRect(0, 0, imageWidth, imageHeight); 
    ctx.fillStyle = 'blue';

    minutiaePoints.forEach(point => {
      const { x, y } = point;
      if (x >= 0 && x < imageWidth && y >= 0 && y < imageHeight) {
        ctx.fillRect(x, y, 3, 3); // Draw a black point
      }
    });
  }, []);

  return (
    <div>
      <canvas className="rounded-xl" ref={canvasRef} width="256" height="288"  />
    </div>
  );
};

export default FingerprintVisualization;
