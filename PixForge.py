import app.py
if __name__ == "__main__":
    # Parse any argument passed from command line
    parser = argparse.ArgumentParser(description='PiCamera2 WebUI')
    parser.add_argument('--port', type=int, default=8080, help='Port number to run the web server on')
    args = parser.parse_args()
    
    app.run(host='0.0.0.0', port=args.port)
