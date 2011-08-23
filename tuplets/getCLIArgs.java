    // Get command line arguments
    for( argi = 0; argi < args.length; argi++ ) {
      if(args[argi] == null) continue;
      String arglows = args[argi].toLowerCase();
      //System.err.println("Args="+arglows);

      if( arglows.startsWith("-ab") ) {
        gufy.rvm_Ab = 0;
        for(int j = 0; j < arglows.length(); j++) {
          if(Character.isDigit(arglows.charAt(j))) {
            String textnums = arglows.substring(j);
            try {
              gufy.rvm_Ab = Integer.parseInt(textnums);
            } catch(Exception e) {
              System.err.println("-Ab value parse error");
              gufy.rvm_Ab = 0;
            }
            break;
          }
        }
        continue;
      }
      if( arglows.startsWith("-ae") ) {
        gufy.rvm_Ae = 0;
        for(int j = 0; j < arglows.length(); j++) {
          if(Character.isDigit(arglows.charAt(j))) {
            String textnums = arglows.substring(j);
            try {
              gufy.rvm_Ae = Integer.parseInt(textnums);
            } catch(Exception e) {
              System.err.println("-Ae value parse error");
              gufy.rvm_Ae = 0;
            }
            break;
          }
        }
        continue;
      }

      if( arglows.startsWith("-cb") ) {
        gufy.rvm_Cb = 0;
        for(int j = 0; j < arglows.length(); j++) {
          if(Character.isDigit(arglows.charAt(j))) {
            String textnums = arglows.substring(j);
            try {
              gufy.rvm_Cb = Integer.parseInt(textnums);
            } catch(Exception e) {
              System.err.println("-Cb value parse error");
              gufy.rvm_Cb = 0;
            }
            break;
          }
        }
        continue;
      }
      if( arglows.startsWith("-ce") ) {
        gufy.rvm_Ce = 0;
        for(int j = 0; j < arglows.length(); j++) {
          if(Character.isDigit(arglows.charAt(j))) {
            String textnums = arglows.substring(j);
            try {
              gufy.rvm_Ce = Integer.parseInt(textnums);
            } catch(Exception e) {
              System.err.println("-Ce value parse error");
              gufy.rvm_Ce = 0;
            }
            break;
          }
        }
        continue;
      }

      if( arglows.startsWith("-fb") ) {
        gufy.rvm_Fb = 0;
        for(int j = 0; j < arglows.length(); j++) {
          if(Character.isDigit(arglows.charAt(j))) {
            String textnums = arglows.substring(j);
            try {
              gufy.rvm_Fb = Integer.parseInt(textnums);
            } catch(Exception e) {
              System.err.println("-Fb value parse error");
              gufy.rvm_Fb = 0;
            }
            break;
          }
        }
        continue;
      }
      if( arglows.startsWith("-fe") ) {
        gufy.rvm_Fe = 0;
        for(int j = 0; j < arglows.length(); j++) {
          if(Character.isDigit(arglows.charAt(j))) {
            String textnums = arglows.substring(j);
            try {
              gufy.rvm_Fe = Integer.parseInt(textnums);
            } catch(Exception e) {
              System.err.println("-Fe value parse error");
              gufy.rvm_Fe = 0;
            }
            break;
          }
        }
        continue;
      }

      if( arglows.startsWith("-fork") ) {
        gufy.fork_Run = true;
        continue;
      }


      if( arglows.startsWith("-help") ||
          arglows.startsWith("-h") ||
          arglows.startsWith("-") ) {

        int cj = 0;
        String Cmds = "GuiffyRVM";
        while(cj < args.length) {
          Cmds = Cmds + " " + args[cj];
          cj++;
        }
        gufy.jifStatus.setText("Command Line:" + Cmds);

        show_Usage(gufy);
        return;
      }

    }
