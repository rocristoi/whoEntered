import React, { useState, useEffect } from 'react';
import FingerprintVisualization from './FingerPrintVis';
import { color, easeOut, motion } from 'framer-motion';

const App = () => {
  const [imageData, setImageData] = useState([]);

  useEffect(() => {
    // Function to fetch data from the API
    const fetchData = () => {
      fetch('http://localhost:3000/getData/')
        .then(response => response.json())
        .then(data => {
          console.log('Fetched data:', data);  // Log the fetched data
          setImageData(data);  // Set image data to state
        })
        .catch(error => {
          console.error('Error fetching data:', error);
        });
    };

    // Fetch data immediately when the component mounts
    fetchData();

    // Set an interval to call fetchData every 5 seconds
    const intervalId = setInterval(fetchData, 5000);

    // Cleanup function to clear the interval when the component unmounts
    return () => clearInterval(intervalId);
  }, []);  // Empty dependency array ensures this effect runs once when the component mounts

  console.log('imageData:', imageData);  // Log imageData state

  

  return (
    <div>
    <div className='mt-10  w-screen flex items-center justify-center'>
      <div className="flex flex-col items-center">
     <h2 className="font-black text-4xl">Who Entered?</h2>
     <h2>Check out who entered using the sensor installed</h2>
     {imageData && (
      <span className='text-sm'>So far, we have registered {imageData.length} entries</span>
     )}
     </div>
     </div>
     <div className="mt-20 flex flex-row flex-wrap gap-10 items-center justify-start m-40 ml-60">
      {imageData && 
        imageData.map((image) => 
          <motion.div className='h-[450px] w-80 bg-[#282828] rounded-xl flex  justify-center' key={image.fingerprint_template}
          initial={{y: -20, opacity: 0}}
          animate={{y: 0, opacity: 1,}}

          >
            <div className='mt-5 flex flex-col items-center '>
            <FingerprintVisualization fingerprintTemplate={image.fingerprint_template} />
            <span className='font-medium text-center'>Registered at {new Date(image.timestamp).toLocaleString()}</span>
            <span className='text-sm p-5 text-center'>This is a visual representation of the user's <a href="https://www.bayometric.com/minutiae-based-extraction-fingerprint-recognition/"><span className='text-blue-500 '>minutiae points</span></a></span>
            <motion.span className='text-sm text-center cursor-pointer' onClick={() => {navigator.clipboard.writeText(image.fingerprint_template)}}
              whileHover={{ scale: 1.1, color: "rgb(59 130 246)" }}
              >Copy raw data </motion.span>

            </div>
          </motion.div>
        )
      }

     </div>
     <div className="flex flex-col items-center leading-tight">
                    <h2>Developed by <span className="text-red-500">@rocristoi</span></h2>
                   <a href="https://github.com/rocristoi/whoEntered"> <h2 className="text-blue-500">Contribute to this project on Github</h2></a>
                  </div>
     </div>
  )
}

export default App
