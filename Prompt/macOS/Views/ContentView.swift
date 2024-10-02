//
//  WelcomeView.swift
//  Prompt
//
//  Created by MakeThingsSimple on 30/9/2024.
//

import SwiftUI

enum NavigationState {
    case welcome, editor
}

struct ContentView: View {
    @StateObject var vm = ViewModel()
    @State var navigationState = NavigationState.welcome
    
    var body: some View {
        Group {
            switch (navigationState) {
            case .welcome:
                VStack(spacing: 20) {
                    Label("Welcome to Prompt", systemImage: "figure.wave")
                        .font(.title3.monospaced())
                        .foregroundStyle(.gray)
                        .textCase(.lowercase)
                    
                    TextField("Type a command to start, e.g. \"open\"", text: $vm.command)
                        .font(.title.monospaced())
                        .textFieldStyle(.plain)
                }
                .padding(20)
            case .editor:
                EditorView()
            }
        }
        .onAppear {
            let root = Bundle.main.resourcePath!
            PythonInterpreter.initialize(root)
        }
        .onDisappear {
            PythonInterpreter.finalize()
        }
    }
}

#Preview {
    ContentView()
}
