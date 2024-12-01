const express = require('express');
const cors = require('cors');  // Import cors package
const app = express();
const port = 3000;
const fs = require('fs');
const path = require('path');

// Enable CORS for all routes
app.use(cors());  // Allow cross-origin requests

app.use(express.json());

// Function to convert base64 to hex
const base64ToHex = (base64) => {
    const buffer = Buffer.from(base64, 'base64');
    return buffer.toString('hex');
};

app.post('/data', (req, res) => {
    const data = req.body;
    console.log('Received data from sensor');

    // Extract fingerprint_template and convert it
    if (data.fingerprint_template) {
        const fingerprintHex = base64ToHex(data.fingerprint_template);

        // Prepare the data to be appended
        const entry = {
            fingerprint_template: fingerprintHex,
            timestamp: new Date().toISOString()
        };

        // Path to the data.json file
        const filePath = path.join(__dirname, 'data.json');

        // Check if the file exists
        fs.exists(filePath, (exists) => {
            if (!exists) {
                // If the file doesn't exist, create it with an empty array
                fs.writeFile(filePath, JSON.stringify([entry], null, 2), (err) => {
                    if (err) {
                        console.error('Error writing file:', err);
                        return res.status(500).send('Internal Server Error');
                    }
                    res.status(200).send('Data received and file created');
                });
            } else {
                // If the file exists, append to it
                fs.readFile(filePath, 'utf8', (err, data) => {
                    if (err) {
                        console.error('Error reading file:', err);
                        return res.status(500).send('Internal Server Error');
                    }

                    // Parse the existing content, append the new entry, and save it back
                    const jsonData = JSON.parse(data);
                    jsonData.push(entry);

                    fs.writeFile(filePath, JSON.stringify(jsonData, null, 2), (err) => {
                        if (err) {
                            console.error('Error writing file:', err);
                            return res.status(500).send('Internal Server Error');
                        }
                        res.status(200).send('Data received and appended to file');
                    });
                });
            }
        });
    } else {
        res.status(400).send('Missing fingerprint_template field');
    }
});

app.get('/getData', (req, res) => {
    const filePath = path.join(__dirname, 'data.json');

    // Read the data.json file and send its contents
    fs.readFile(filePath, 'utf8', (err, data) => {
        if (err) {
            console.error('Error reading file:', err);
            return res.status(500).send('Internal Server Error');
        }

        // Return the content of data.json
        res.status(200).json(JSON.parse(data));
    });
});

app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});
