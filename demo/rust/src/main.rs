// Chan Signal
#[macro_use]
extern crate chan;
extern crate chan_signal;
use chan_signal::Signal;

// Clap
extern crate clap;
use clap::{Arg, App};

// cpal
extern crate cpal;

// Porcupine
extern crate porcupine_sys;
use porcupine_sys as pv;

use std::sync::Arc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::thread;

fn main() {
    // Setup command-line options
    let matches = App::new("Porcupine Rust Demo")
            .version("0.1.0")
            .about("This is a simple Rust program that demonstrates how to use Porcupine")
            .arg(Arg::with_name("model-file-path").short("m").long("model-file-path").value_name("FILE").help("Sets the model file path").takes_value(true))
            .arg(Arg::with_name("keyword-file-path").short("k").long("keyword-file-path").value_name("FILE").help("Sets the keyword file path(s)").takes_value(true).multiple(true))
            .get_matches();
    let model_file_path = matches.value_of("model-file-path");
    if model_file_path.is_none() {
        println!("Must provide path to the model file!");
        return;
    }
    let keyword_file_paths = matches.values_of("keyword-file-path");
    if keyword_file_paths.is_none() {
        println!("Must provide paths to the keyword file(s)!");
        return;
    }

    // Set model file path
    let model_file_path = model_file_path.unwrap();

    // Set keyword file paths
    let keyword_file_paths: Vec<String> = keyword_file_paths.unwrap().map(|val| val.to_string()).collect();

    // Set sensitivities
    let sensitivities: Vec<f32> = keyword_file_paths.iter().map(|_val| 0.5).collect();

    // Setup microphone
    let device = cpal::default_input_device().expect("Failed to get default input device");
    let format = cpal::Format { channels: 1, sample_rate: cpal::SampleRate(16000), data_type: cpal::SampleFormat::I16 };
    let event_loop = Arc::new(cpal::EventLoop::new());
    let stream_id = event_loop.build_input_stream(&device, &format)
        .expect("Failed to build input stream");
    event_loop.play_stream(stream_id.clone());

    // Print microphone information
    println!("Default input device: {}", device.name());
    println!("Default input format: {:?}", format);

    // Setup OS signals
    let signal = chan_signal::notify(&[Signal::INT, Signal::TERM]);

    // Setup Porcupine
    let frame_length = unsafe { pv::frame_length() };
    let object =
        if keyword_file_paths.len() == 1 {
            unsafe { pv::Object::new(&model_file_path, &keyword_file_paths[0], sensitivities[0]).unwrap() }
        } else {
            let keyword_file_paths: Vec<&str> = keyword_file_paths.iter().map(|s| s.as_str()).collect();
            unsafe { pv::Object::new_multiple_keywords(&model_file_path, &keyword_file_paths, &sensitivities).unwrap() }
        };

    // Start the microphone and detect keyword
    let recording = Arc::new(AtomicBool::new(true));
    let recording_1 = recording.clone();
    let event_loop_1 = event_loop.clone();
    thread::spawn(move || {
        let mut samples_buffer = Vec::new();

        // Run the microphone event loop
        event_loop_1.run(move |_, data| {
            // If we're done recording, return early.
            if !recording_1.load(Ordering::Relaxed) {
                return;
            }

            // Otherwise process audio for keyword detection
            if let cpal::StreamData::Input { buffer: cpal::UnknownTypeInputBuffer::I16(buffer) } = data {
                let samples: Vec<i16> = buffer.iter().cloned().collect();

                // Add audio data to the buffer
                samples_buffer.extend_from_slice(&samples);

                // Process buffer when it reaches Porcupine frame length
                if samples_buffer.len() >= frame_length {
                    let data: Vec<i16> = samples_buffer.drain(..frame_length).collect();
                    if keyword_file_paths.len() == 1 {
                        if let Ok(detected) = unsafe { object.process(&data) } {
                            if detected {
                                println!("Detected keyword!");
                            }
                        }
                    } else {
                        if let Ok(detected) = unsafe { object.process_multiple_keywords(&data) } {
                            if detected >= 0 {
                                println!("Detected keyword at index {}!", detected);
                            }
                        }
                    }
                }
            }
        });
    });

    // Wait for an OS signal
    chan_select! {
        signal.recv() -> signal => {
            println!("received signal: {:?}", signal);
            recording.store(false, Ordering::Relaxed);
            event_loop.destroy_stream(stream_id.clone());
        },
    }
}
