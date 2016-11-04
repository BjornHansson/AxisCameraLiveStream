package se.mah.axis.camera;

import java.awt.BorderLayout;
import java.awt.EventQueue;
import java.awt.FlowLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.SpinnerNumberModel;

/**
 * Main entry point to start and create the client GUI.
 */
public class Gui {

    private Controller controller;

    private JFrame frame;
    private JTextField tfCameraIp;
    private JTextField tfCameraPort;
    private JComboBox<String> resolutionComboBox;
    private JSpinner frameRateSpinner;
    private JLabel lblCameraIp;
    private JLabel lblCameraPort;
    private JLabel lblResolution;
    private JLabel lblFrameRate;
    private JToggleButton connectToggleButton;
    private JLabel lblContainer;
    private JPasswordField tfPassword;
    private JLabel lblPassword;

    /**
     * Launch the application.
     */
    public static void main(String[] args) {
        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Gui window = new Gui();
                    window.frame.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Create the application.
     */
    public Gui() {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize() {
        frame = new JFrame("Axis Camera Client User Interface");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.getContentPane().setLayout(new BorderLayout(2, 1));
        frame.setExtendedState(frame.getExtendedState() | JFrame.MAXIMIZED_BOTH);

        JPanel northPanel = new JPanel(new FlowLayout());
        JPanel southPanel = new JPanel(new FlowLayout());

        lblCameraIp = new JLabel("Camera IP");
        northPanel.add(lblCameraIp);

        tfCameraIp = new JTextField();
        tfCameraIp.setText("192.168.2.101");
        northPanel.add(tfCameraIp);
        tfCameraIp.setColumns(10);

        lblCameraPort = new JLabel("Camera port");
        northPanel.add(lblCameraPort);

        tfCameraPort = new JTextField();
        tfCameraPort.setText("5005");
        northPanel.add(tfCameraPort);
        tfCameraPort.setColumns(10);

        lblPassword = new JLabel("Password");
        northPanel.add(lblPassword);

        tfPassword = new JPasswordField();
        tfPassword.setColumns(10);
        northPanel.add(tfPassword);

        lblResolution = new JLabel("Resolution");
        northPanel.add(lblResolution);

        resolutionComboBox = new JComboBox<String>();
        resolutionComboBox.setModel(new DefaultComboBoxModel(new String[] { "320x180", "480x270", "640x360", "800x450" }));
        northPanel.add(resolutionComboBox);

        lblFrameRate = new JLabel("Frame rate");
        northPanel.add(lblFrameRate);

        frameRateSpinner = new JSpinner();
        frameRateSpinner.setModel(new SpinnerNumberModel(10, 1, 25, 1));
        northPanel.add(frameRateSpinner);

        connectToggleButton = new JToggleButton("Connect");
        northPanel.add(connectToggleButton);

        lblContainer = new JLabel("Placeholder for image");
        southPanel.add(lblContainer);

        frame.getContentPane().add(northPanel, BorderLayout.NORTH);
        frame.getContentPane().add(southPanel, BorderLayout.CENTER);

        addMouseListeners();
    }

    private void enableEditableElements(boolean enable) {
        if (enable) {
            connectToggleButton.setText("Connect");
        } else {
            connectToggleButton.setText("Disconnect");
        }
        tfCameraIp.setEditable(enable);
        tfCameraPort.setEditable(enable);
        resolutionComboBox.setEnabled(enable);
        frameRateSpinner.setEnabled(enable);
        // Performs re-layout of the frame
        frame.validate();
    }

    private void addMouseListeners() {
        // Listen on the connect/disconnect button
        connectToggleButton.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {

                if (connectToggleButton.isSelected()) {
                    // Connect clicked
                    controller = new Controller();
                    controller.setCameraIp(tfCameraIp.getText());
                    controller.setCameraPort(Integer.parseInt(tfCameraPort.getText()));
                    controller.setResolution(String.valueOf(resolutionComboBox.getSelectedIndex()));
                    controller.setFps(frameRateSpinner.getValue().toString());
                    controller.setPassword(tfPassword.getText());

                    // Java 8 way of writing. Lambda expression
                    Thread t = new Thread((Runnable) () -> {
                        try {
                            lblContainer.setText("Connecting to " + tfCameraIp.getText() + ":" + String.valueOf(tfCameraPort.getText()) + " ...");
                            controller.connectToCamera(lblContainer);
                        } catch (IOException e1) {
                            lblContainer.setText("<html><font color='red'>" + e1.getMessage() + "</font>, please make sure you are connected to the camera and then try again.</html>");
                            e1.printStackTrace();
                            connectToggleButton.setSelected(false);
                            enableEditableElements(true);
                        }
                    });
                    t.start();

                    // Camera connected so disable fields
                    enableEditableElements(false);
                } else {
                    // Disconnect clicked so enable fields and close socket
                    connectToggleButton.setSelected(false);
                    enableEditableElements(true);
                    try {
                        controller.disconnectCamera();
                    } catch (IOException e1) {
                        lblContainer.setText(e1.getMessage() + ", something did not go the way it was suppose to.");
                        e1.printStackTrace();
                    }
                }
            }
        });
    }
}
