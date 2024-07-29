pipeline {
    agent any

    stages {
        stage('Clone Repository') {
            steps {
                script {
                    // Clone the repository
                    git url: 'https://github.com/ToneGabriel/CustomSTL', branch: 'main'
                }
            }
        }
        stage('Build Docker Image') {
            steps {
                script {
                    // Build Docker image using Dockerfile in the repository
                    docker.build('custom_stl_linux', '-f dockerfile.linux .')
                }
            }
        }
        stage('Run Docker Container and Build Project') {
            steps {
                script {
                    // Run Docker container and execute build commands inside it
                    docker.image('custom_stl_linux').inside {
                        sh 'echo "Building project inside Docker container"'
                        sh 'cmake .'
                        sh 'make'
                    }
                }
            }
        }
    }

    // stages {
    //     stage('Clone Repository') {
    //         steps {
    //             script {
    //                 // Clone the repository
    //                 git url: 'https://github.com/ToneGabriel/CustomSTL', branch: 'main'
    //             }
    //         }
    //     }
    //     stage('Print Git Version') {
    //         steps {
    //             script {
    //                 // Print Git version
    //                 sh 'git --version'
    //             }
    //         }
    //     }
    //     stage('List Repository Contents') {
    //         steps {
    //             script {
    //                 // List contents of the repository
    //                 sh 'ls -l'
    //             }
    //         }
    //     }
    // }
}
