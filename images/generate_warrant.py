from reportlab.lib.pagesizes import letter
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Image, Table, TableStyle
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib import colors
import os

def generate_warrant():
    doc_path = "/Users/rawadfahd/Projects/5M copy/images/arrest_warrant.pdf"
    image_path = "/Users/rawadfahd/Projects/5M copy/View recent photos.png"
    
    doc = SimpleDocTemplate(doc_path, pagesize=letter)
    styles = getSampleStyleSheet()
    
    # Custom Styles
    header_style = ParagraphStyle(
        'HeaderStyle',
        parent=styles['Heading1'],
        fontSize=18,
        spaceAfter=12,
        alignment=1, # Center
        fontName='Helvetica-Bold'
    )
    
    sub_header_style = ParagraphStyle(
        'SubHeaderStyle',
        parent=styles['Normal'],
        fontSize=12,
        spaceAfter=10,
        alignment=1,
        fontName='Helvetica-Bold'
    )
    
    label_style = ParagraphStyle(
        'LabelStyle',
        parent=styles['Normal'],
        fontSize=10,
        fontName='Helvetica-Bold'
    )
    
    charge_style = ParagraphStyle(
        'ChargeStyle',
        parent=styles['Normal'],
        fontSize=11,
        leading=14,
        textColor=colors.black
    )

    story = []

    # Header
    story.append(Paragraph("SUPERIOR COURT OF THE STATE", header_style))
    story.append(Paragraph("CRIMINAL DIVISION - BUREAU OF ENFORCEMENT", sub_header_style))
    story.append(Spacer(1, 20))
    
    # Case Info
    data = [
        [Paragraph("CASE NO: 2024-CR-58291-B", label_style), Paragraph("DATE: FEBRUARY 08, 2026", label_style)],
        [Paragraph("DEFENDANT: [NAME REDACTED]", label_style), Paragraph("STATUS: WARRANT ACTIVE", label_style)]
    ]
    t = Table(data, colWidths=[300, 200])
    t.setStyle(TableStyle([('ALIGN', (0, 0), (-1, -1), 'LEFT'), ('VALIGN', (0, 0), (-1, -1), 'TOP')]))
    story.append(t)
    story.append(Spacer(1, 25))

    # Mugshot Image
    if os.path.exists(image_path):
        img = Image(image_path, width=2.5*72, height=3.5*72) # 2.5x3.5 inches
        img.hAlign = 'CENTER'
        story.append(img)
        story.append(Paragraph("DEFENDANT IDENTIFICATION PHOTO", sub_header_style))
    story.append(Spacer(1, 30))

    # Charges Section
    story.append(Paragraph("CRIMINAL COMPLAINT AND FORMAL CHARGES", sub_header_style))
    story.append(Spacer(1, 10))
    
    charges = [
        "<b>COUNT 1: PUBLIC INDECENCY (FELONY)</b><br/>The defendant did willfully and knowingly expose themselves in a public setting with the intent to cause alarm or scandal, in violation of State Penal Code Section 415.2.",
        "<b>COUNT 2: HUMAN TRAFFICKING AND EXPLOITATION (18 U.S.C. § 1591)</b><br/>The defendant is charged with the transport, recruitment, and harboring of individuals for the purpose of forced labor and sexual exploitation, specifically targeting minors and vulnerable populations.",
        "<b>COUNT 3: CONSPIRACY TO COMMIT CHILD EXPLOITATION</b><br/>Agreement with one or more persons to facilitate and promote the illegal distribution of prohibited materials and the exploitation of minors."
    ]
    
    for charge in charges:
        story.append(Paragraph(charge, charge_style))
        story.append(Spacer(1, 15))

    story.append(Spacer(1, 40))

    # Probable Cause / Signature Section
    story.append(Paragraph("PROBABLE CAUSE AFFIDAVIT", label_style))
    story.append(Paragraph("Based on surveillance footage, flight log documentation recovered from recent digital forensic analysis, and witness testimony, there is sufficient evidence to believe that the defendant committed the aforementioned acts. This warrant is issued for immediate execution.", styles['Normal']))
    
    story.append(Spacer(1, 50))
    
    # Signatures
    sig_data = [
        [Paragraph("__________________________", label_style), Paragraph("__________________________", label_style)],
        [Paragraph("JUDGE / MAGISTRATE", label_style), Paragraph("REPORTING OFFICER", label_style)]
    ]
    sig_t = Table(sig_data, colWidths=[250, 250])
    sig_t.setStyle(TableStyle([('ALIGN', (0, 0), (-1, -1), 'CENTER')]))
    story.append(sig_t)

    # Build PDF
    doc.build(story)
    print(f"Generated PDF at {doc_path}")

if __name__ == "__main__":
    generate_warrant()
